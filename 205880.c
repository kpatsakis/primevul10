void Item_field::make_field(Send_field *tmp_field)
{
  field->make_field(tmp_field);
  DBUG_ASSERT(tmp_field->table_name != 0);
  if (name)
    tmp_field->col_name=name;			// Use user supplied name
  if (table_name)
    tmp_field->table_name= table_name;
  if (db_name)
    tmp_field->db_name= db_name;
}