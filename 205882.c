Item_equal *Item_direct_view_ref::find_item_equal(COND_EQUAL *cond_equal)
{
  Item* field_item= real_item();
  if (field_item->type() != FIELD_ITEM)
    return NULL;
  return ((Item_field *) field_item)->find_item_equal(cond_equal);  
}