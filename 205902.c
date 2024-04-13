Item *Item_direct_view_ref::equal_fields_propagator(uchar *arg)
{
  Item *field_item= real_item();
  if (field_item->type() != FIELD_ITEM)
    return this;
  Item *item= field_item->equal_fields_propagator(arg);
  set_item_equal(field_item->get_item_equal());
  field_item->set_item_equal(NULL);
  if (item != field_item)
    return item;
  return this;
}