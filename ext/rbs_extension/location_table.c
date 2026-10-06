#include "location_table.h"
#include "class_constants.h"
#include "legacy_location.h"
#include "rbs_extension.h"

typedef struct {
    int32_t *data;
    size_t len;
    size_t cap;
} rbs_location_table_t;

static VALUE RBS_LocationTable;
static ID id_location_table;

static void location_table_free(void *ptr) {
    rbs_location_table_t *table = (rbs_location_table_t *) ptr;
    free(table->data);
    ruby_xfree(table);
}

static size_t location_table_memsize(const void *ptr) {
    const rbs_location_table_t *table = (const rbs_location_table_t *) ptr;
    return sizeof(rbs_location_table_t) + table->cap * sizeof(int32_t);
}

static const rb_data_type_t location_table_type = {
    "RBS::LocationTable",
    { NULL, location_table_free, location_table_memsize },
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY
};

static rbs_location_table_t *check_location_table(VALUE table) {
    return (rbs_location_table_t *) rb_check_typeddata(table, &location_table_type);
}

VALUE rbs_location_table_for(VALUE buffer) {
    // A hidden ivar (no `@`), so it doesn't show up in `Buffer#instance_variables`.
    VALUE table = rb_attr_get(buffer, id_location_table);

    if (NIL_P(table)) {
        rbs_location_table_t *data;
        table = TypedData_Make_Struct(RBS_LocationTable, rbs_location_table_t, &location_table_type, data);
        *data = (rbs_location_table_t) { .data = NULL, .len = 0, .cap = 0 };
        rb_ivar_set(buffer, id_location_table, table);
    }

    return table;
}

VALUE rbs_location_table_append(VALUE table, int node_type, rbs_location_range base, const rbs_location_range *children, unsigned short count) {
    if (RBS_LOCATION_NULL_RANGE_P(base)) {
        return Qnil;
    }

    rbs_location_table_t *data = check_location_table(table);
    size_t size = 3 + 2 * (size_t) count;

    if (data->len + size > data->cap) {
        size_t cap = data->cap ? data->cap * 2 : 1024;
        while (cap < data->len + size) {
            cap *= 2;
        }
        data->data = (int32_t *) realloc(data->data, cap * sizeof(int32_t));
        data->cap = cap;
    }

    size_t index = data->len;
    int32_t *record = data->data + index;
    record[0] = node_type;
    record[1] = base.start_char;
    record[2] = base.end_char;
    for (unsigned short i = 0; i < count; i++) {
        record[3 + 2 * i] = children[i].start_char;
        record[4 + 2 * i] = children[i].end_char;
    }
    data->len += size;

    return SIZET2NUM(index);
}

void rbs_location_table_shrink(VALUE table) {
    rbs_location_table_t *data = check_location_table(table);

    if (data->len > 0 && data->len < data->cap) {
        data->data = (int32_t *) realloc(data->data, data->len * sizeof(int32_t));
        data->cap = data->len;
    }
}

/**
 * `RBS::LocationTable.__materialize(buffer, index)`
 *
 * Builds the same `RBS::Location` the parser used to build eagerly.
 */
static VALUE location_table_s_materialize(VALUE klass, VALUE buffer, VALUE index) {
    VALUE table = rb_attr_get(buffer, id_location_table);
    if (NIL_P(table)) {
        rb_raise(rb_eRuntimeError, "buffer has no location table");
    }

    rbs_location_table_t *data = check_location_table(table);
    size_t i = NUM2SIZET(index);
    if (i + 3 > data->len) {
        rb_raise(rb_eIndexError, "location index out of range: %zu", i);
    }

    const int32_t *record = data->data + i;
    const rbs_location_schema_t *schema = rbs_location_schema_for(record[0]);
    if (schema == NULL) {
        rb_raise(rb_eRuntimeError, "no location schema for node type %d", record[0]);
    }

    VALUE location = rbs_new_location2(buffer, record[1], record[2]);
    rbs_loc *loc = rbs_check_location(location);
    rbs_loc_legacy_alloc_children(loc, schema->count);

    for (unsigned short k = 0; k < schema->count; k++) {
        rbs_loc_range range = { .start = record[3 + 2 * k], .end = record[4 + 2 * k] };
        ID name = rb_intern(schema->entries[k].name);
        if (schema->entries[k].required) {
            rbs_loc_legacy_add_required_child(loc, name, range);
        } else {
            rbs_loc_legacy_add_optional_child(loc, name, range);
        }
    }

    return location;
}

void rbs__init_location_table(void) {
    id_location_table = rb_intern("__location_table");

    RBS_LocationTable = rb_define_class_under(RBS, "LocationTable", rb_cObject);
    rb_undef_alloc_func(RBS_LocationTable);
    rb_define_singleton_method(RBS_LocationTable, "__materialize", location_table_s_materialize, 2);
}
