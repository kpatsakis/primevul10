Item *Item_field::safe_charset_converter(CHARSET_INFO *tocs)
{
  no_const_subst= 1;
  return Item::safe_charset_converter(tocs);
}