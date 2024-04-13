bool Item_field::add_field_to_set_processor(uchar *arg)
{
  DBUG_ENTER("Item_field::add_field_to_set_processor");
  DBUG_PRINT("info", ("%s", field->field_name ? field->field_name : "noname"));
  TABLE *table= (TABLE *) arg;
  if (field->table == table)
    bitmap_set_bit(&table->tmp_set, field->field_index);
  DBUG_RETURN(FALSE);
}