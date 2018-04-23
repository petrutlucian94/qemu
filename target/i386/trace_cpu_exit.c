#include "trace_cpu_exit.h"

#include "qemu/osdep.h"
#include "exec/address-spaces.h"
#include "qemu-common.h"
#include "exec/hwaddr.h"

#include "glib.h"
#include <stdio.h>
#include <stdbool.h>

static GHashTable **stats_tables;
static int _vcpu_count;

static GHashTable **ioport_stats_tables;
static GHashTable **mmio_stats_tables;

void trace_cpu_init(int vcpu_count) {
    static GHashTable*** tables[] = {&stats_tables, &ioport_stats_tables, &mmio_stats_tables};
    int i, table_it;
    _vcpu_count = vcpu_count;

    for (table_it=0; table_it<3; table_it++) {
        *(tables[table_it]) = (GHashTable**) malloc(sizeof(GHashTable*) * _vcpu_count);

        //fprintf(stdout, "Initializing tables %d.\n", table_it);
        for (i=0; i<_vcpu_count; i++) {
            // fprintf(stdout, "Initializing hashtable %d:%d.\n", table_it, i);
            (*(tables[table_it]))[i] = g_hash_table_new_full(
                g_direct_hash, g_direct_equal, NULL, NULL);
        }
    }

    fprintf(stdout, "Trace initialized. Cpu count: %d\n", vcpu_count);
}

void trace_cpu_push_event(int cpu_id, int exit_type, LONGLONG elapsedMicrosec) {
    // fprintf(stdout, "Pushing event: %d/%d %d.\n", cpu_id, _vcpu_count, exit_type);
    GHashTable* table = stats_tables[cpu_id];
    EXIT_STATS* exit_stats;

    exit_stats = g_hash_table_lookup(table, GINT_TO_POINTER(exit_type));
    if (!exit_stats){
        exit_stats = g_malloc0(sizeof(EXIT_STATS));
        exit_stats->min_duration = 0xfffff;
        g_hash_table_insert (table,
                             GINT_TO_POINTER(exit_type),
                             exit_stats);
    }
    exit_stats->exit_type = exit_type;
    exit_stats->call_count += 1;
    exit_stats->min_duration = MIN(exit_stats->min_duration, elapsedMicrosec);
    exit_stats->max_duration = MAX(exit_stats->max_duration, elapsedMicrosec);
    exit_stats->total_duration += elapsedMicrosec;
}

static gint gint_compare(gconstpointer ptr_a, gconstpointer ptr_b)  
{  
    gint a, b; 
    a = GPOINTER_TO_INT(ptr_a);  
    b = GPOINTER_TO_INT(ptr_b);  

    if (a > b)  { return (1); }  
    if (a == b) { return (0); }    
    return (-1);  
}

static gint gint_strcmp(gconstpointer ptr_a, gconstpointer ptr_b)
{
    char *a = (char *) ptr_a;
    char *b = (char *) ptr_b;

    return strcmp(a, b);
}

void trace_cpu_print_cpu_stats(int cpu_id) {
    EXIT_STATS* exit_stats;
    GHashTable* table = stats_tables[cpu_id];

    GList* exit_keys = g_hash_table_get_keys(table);
    exit_keys = g_list_sort(exit_keys, gint_compare);
    GList* exit_iter = exit_keys;

    fprintf(stdout, "VCPU %d exit stats\n", cpu_id);
    fprintf(stdout, "==================\n");
    fprintf(stdout, "TYPE COUNT MIN MAX TOTAL AVG\n");
    while (exit_iter) {
        exit_stats = g_hash_table_lookup(table, GINT_TO_POINTER(exit_iter->data));
        fprintf(stdout, "%d %d %.6fs %.6fs %.6fs %.6fs\n",
                exit_stats->exit_type,
                exit_stats->call_count,
                exit_stats->min_duration / 1000000.0,
                exit_stats->max_duration / 1000000.0,
                exit_stats->total_duration / 1000000.0,
                exit_stats->total_duration / exit_stats->call_count / 1000000.0);
        exit_iter = exit_iter->next;
    }
    fprintf(stdout, "==================\n\n");
}

static void trace_print_dev_access_stats(int cpu_id, GHashTable** tables, char* dev_access_type) {
    DEV_ACCESS_STATS* access_stats;
    GHashTable* table = tables[cpu_id];

    GList* dev_keys = g_hash_table_get_keys(table);
    dev_keys = g_list_sort(dev_keys, gint_strcmp);
    GList* dev_iter = dev_keys;

    fprintf(stdout, "VCPU %d %s stats\n", cpu_id, dev_access_type);
    fprintf(stdout, "==================\n");
    fprintf(stdout, "TYPE COUNT READ WRITE MIN MAX TOTAL AVG\n");
    while (dev_iter) {
        access_stats = g_hash_table_lookup(table, dev_iter->data);
        fprintf(stdout, "%s %d %d %d %.6fs %.6fs %.6fs %.6fs\n",
                access_stats->dev_name,
                access_stats->call_count,
                access_stats->call_count - access_stats->write_count,
                access_stats->write_count,
                access_stats->min_duration / 1000000.0,
                access_stats->max_duration / 1000000.0,
                access_stats->total_duration / 1000000.0,
                access_stats->total_duration / access_stats->call_count / 1000000.0);
        dev_iter = dev_iter->next;
    }
    fprintf(stdout, "==================\n\n");
}

void trace_cpu_print_stats(void) {
    int i;
    for (i=0; i<_vcpu_count; i++) {
        trace_cpu_print_cpu_stats(i);
        trace_print_dev_access_stats(i, mmio_stats_tables, "MMIO");
        trace_print_dev_access_stats(i, ioport_stats_tables, "IOPort");
    }

    // mtree_info(fprintf, stdout, false);
}

static void trace_device_access_event(
                               GHashTable** stats_table_array,
                               AddressSpace *as,
                               int cpu_id, hwaddr addr, int len,
                               bool is_write, LONGLONG elapsedMicrosec) {
    MemoryRegion *mr;
    hwaddr l;
    hwaddr addr1;

    GHashTable* table = stats_table_array[cpu_id];
    DEV_ACCESS_STATS* access_stats;

    rcu_read_lock();
    l = len;
    mr = address_space_translate(as, addr, &addr1, &l, is_write);
    rcu_read_unlock();

    if (l!=len) {
        fprintf(stdout, "access length %d != %ld\n", len, (long)l);
    }

    #define T_DEV_NAME (mr->name) ? mr->name : "undefined"

    access_stats = g_hash_table_lookup(table, T_DEV_NAME);
    if (!access_stats){
        access_stats = g_malloc0(sizeof(DEV_ACCESS_STATS));
        access_stats->min_duration = 0xfffff;
        g_hash_table_insert(table,
                            T_DEV_NAME,
                            access_stats);
    }
    if (is_write)
        access_stats->write_count += 1;

    access_stats->dev_name = T_DEV_NAME;
    access_stats->call_count += 1;
    access_stats->min_duration = MIN(access_stats->min_duration, elapsedMicrosec);
    access_stats->max_duration = MAX(access_stats->max_duration, elapsedMicrosec);
    access_stats->total_duration += elapsedMicrosec;
}

void trace_mmio_event(int cpu_id, hwaddr addr, int len,
                      bool is_write, LONGLONG elapsedMicrosec) {
    trace_device_access_event(mmio_stats_tables, &address_space_memory,
                              cpu_id, addr, len, is_write, elapsedMicrosec);
}

void trace_ioport_event(int cpu_id, hwaddr addr, int len,
                        bool is_write, LONGLONG elapsedMicrosec) {
    trace_device_access_event(ioport_stats_tables, &address_space_io,
                              cpu_id, addr, len, is_write, elapsedMicrosec);
}
