bool Item_string::eq(const Item *item, bool binary_cmp) const
{
  if (type() == item->type() && item->basic_const_item())
  {
    if (binary_cmp)
      return !stringcmp(&str_value, &item->str_value);
    return (collation.collation == item->collation.collation &&
	    !sortcmp(&str_value, &item->str_value, collation.collation));
  }
  return 0;
}