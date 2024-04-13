void Item_copy_decimal::copy()
{
  my_decimal *nr= item->val_decimal(&cached_value);
  if (nr && nr != &cached_value)
    my_decimal2decimal (nr, &cached_value);
  null_value= item->null_value;
}