bool Item_int::eq(const Item *arg, bool binary_cmp) const
{
  /* No need to check for null value as basic constant can't be NULL */
  if (arg->basic_const_item() && arg->type() == type())
  {
    /*
      We need to cast off const to call val_int(). This should be OK for
      a basic constant.
    */
    Item *item= (Item*) arg;
    return (item->val_int() == value &&
            ((longlong) value >= 0 ||
             (item->unsigned_flag == unsigned_flag)));
  }
  return FALSE;
}