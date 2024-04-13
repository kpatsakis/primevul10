bool Item_null::eq(const Item *item, bool binary_cmp) const
{ return item->type() == type(); }