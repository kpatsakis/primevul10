Item *Item_direct_view_ref::replace_equal_field(uchar *arg)
{
  Item *field_item= real_item();
  if (field_item->type() != FIELD_ITEM)
    return this;
  field_item->set_item_equal(item_equal);
  Item *item= field_item->replace_equal_field(arg);
  field_item->set_item_equal(0);
  return item != field_item ? item : this;
}