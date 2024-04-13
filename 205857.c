int Item::save_in_field_no_warnings(Field *field, bool no_conversions)
{
  int res;
  TABLE *table= field->table;
  THD *thd= table->in_use;
  enum_check_fields tmp= thd->count_cuted_fields;
  my_bitmap_map *old_map= dbug_tmp_use_all_columns(table, table->write_set);
  ulonglong sql_mode= thd->variables.sql_mode;
  thd->variables.sql_mode&= ~(MODE_NO_ZERO_IN_DATE | MODE_NO_ZERO_DATE);
  thd->variables.sql_mode|= MODE_INVALID_DATES;
  thd->count_cuted_fields= CHECK_FIELD_IGNORE;

  res= save_in_field(field, no_conversions);

  thd->count_cuted_fields= tmp;
  dbug_tmp_restore_column_map(table->write_set, old_map);
  thd->variables.sql_mode= sql_mode;
  return res;
}