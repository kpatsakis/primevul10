bool Item_field::register_field_in_bitmap(uchar *arg)
{
  MY_BITMAP *bitmap= (MY_BITMAP *) arg;
  DBUG_ASSERT(bitmap);
  bitmap_set_bit(bitmap, field->field_index);
  return 0;
}