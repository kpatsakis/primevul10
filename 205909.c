bool Item_direct_view_ref::eq(const Item *item, bool binary_cmp) const
{
  if (item->type() == REF_ITEM)
  {
    Item_ref *item_ref= (Item_ref*) item;
    if (item_ref->ref_type() == VIEW_REF)
    {
      Item *item_ref_ref= *(item_ref->ref);
      return ((*ref)->real_item() == item_ref_ref->real_item());
    }
  }
  return FALSE;
}