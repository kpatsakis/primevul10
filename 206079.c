void Item_copy_int::copy()
{
  cached_value= item->val_int();
  null_value=item->null_value;
}