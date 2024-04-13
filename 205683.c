Field *Item_type_holder::make_field_by_type(TABLE *table)
{
  /*
    The field functions defines a field to be not null if null_ptr is not 0
  */
  uchar *null_ptr= maybe_null ? (uchar*) "" : 0;
  Field *field;

  switch (fld_type) {
  case MYSQL_TYPE_ENUM:
    DBUG_ASSERT(enum_set_typelib);
    field= new Field_enum((uchar *) 0, max_length, null_ptr, 0,
                          Field::NONE, name,
                          get_enum_pack_length(enum_set_typelib->count),
                          enum_set_typelib, collation.collation);
    if (field)
      field->init(table);
    return field;
  case MYSQL_TYPE_SET:
    DBUG_ASSERT(enum_set_typelib);
    field= new Field_set((uchar *) 0, max_length, null_ptr, 0,
                         Field::NONE, name,
                         get_set_pack_length(enum_set_typelib->count),
                         enum_set_typelib, collation.collation);
    if (field)
      field->init(table);
    return field;
  case MYSQL_TYPE_NULL:
    return make_string_field(table);
  default:
    break;
  }
  return tmp_table_field_from_field_type(table, 0);
}