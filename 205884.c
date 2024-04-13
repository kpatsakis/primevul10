bool Item_field::register_field_in_read_map(uchar *arg)
{
  TABLE *table= (TABLE *) arg;
  if (field->table == table || !table)
    bitmap_set_bit(field->table->read_set, field->field_index);
  if (field->vcol_info && field->vcol_info->expr_item)
    return field->vcol_info->expr_item->walk(&Item::register_field_in_read_map, 
                                             1, arg);
  return 0;
}