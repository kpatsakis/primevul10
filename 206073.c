Field *Item::make_string_field(TABLE *table)
{
  Field *field;
  DBUG_ASSERT(collation.collation);
  /* 
    Note: the following check is repeated in 
    subquery_types_allow_materialization():
  */
  if (too_big_for_varchar())
    field= new Field_blob(max_length, maybe_null, name,
                          collation.collation, TRUE);
  /* Item_type_holder holds the exact type, do not change it */
  else if (max_length > 0 &&
      (type() != Item::TYPE_HOLDER || field_type() != MYSQL_TYPE_STRING))
    field= new Field_varstring(max_length, maybe_null, name, table->s,
                               collation.collation);
  else
    field= new Field_string(max_length, maybe_null, name,
                            collation.collation);
  if (field)
    field->init(table);
  return field;
}