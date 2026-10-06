#ifndef RBS_LOCATION_TABLE_H
#define RBS_LOCATION_TABLE_H

#include "compat.h"

SUPPRESS_RUBY_HEADER_DIAGNOSTICS_BEGIN
#include "ruby.h"
SUPPRESS_RUBY_HEADER_DIAGNOSTICS_END

#include "rbs.h"

/**
 * Prototype: per-buffer table of node locations.
 *
 * Instead of an `RBS::Location` object per node, the parser appends a record
 * `[node_type, start_char, end_char, (child_start, child_end) * N]` to an
 * int32 array attached to the buffer, and the node keeps the record index.
 * Child names come from a static schema per node type.
 */

typedef struct {
    const char *name;
    bool required;
} rbs_location_schema_entry_t;

typedef struct {
    unsigned short count;
    const rbs_location_schema_entry_t *entries;
} rbs_location_schema_t;

/** Defined in the generated `ast_translation.c`. */
const rbs_location_schema_t *rbs_location_schema_for(int node_type);

/** Returns the table attached to `buffer`, creating it on first use. */
VALUE rbs_location_table_for(VALUE buffer);

/** Appends a record and returns its index as an Integer, or nil for a null range. */
VALUE rbs_location_table_append(VALUE table, int node_type, rbs_location_range base, const rbs_location_range *children, unsigned short count);

/** Drops the spare capacity left by geometric growth. */
void rbs_location_table_shrink(VALUE table);

void rbs__init_location_table(void);

#endif
