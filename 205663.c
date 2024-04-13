void Item_ref::make_field(Send_field *field)
{
  (*ref)->make_field(field);
  /* Non-zero in case of a view */
  if (name)
    field->col_name= name;
  if (table_name)
    field->table_name= table_name;
  if (db_name)
    field->db_name= db_name;
  if (orig_field_name)
    field->org_col_name= orig_field_name;
  if (orig_table_name)
    field->org_table_name= orig_table_name;
}