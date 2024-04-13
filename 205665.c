bool Item::eq(const Item *item, bool binary_cmp) const
{
  /*
    Note, that this is never TRUE if item is a Item_param:
    for all basic constants we have special checks, and Item_param's
    type() can be only among basic constant types.
  */
  return type() == item->type() && name && item->name &&
    !my_strcasecmp(system_charset_info,name,item->name);
}