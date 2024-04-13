bool Item_field::register_field_in_write_map(uchar *arg)
{
  TABLE *table= (TABLE *) arg;
  if (field->table == table || !table)
    bitmap_set_bit(field->table->write_set, field->field_index);
  return 0;
}