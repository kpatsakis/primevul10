Item *Item_null::safe_charset_converter(CHARSET_INFO *tocs)
{
  collation.set(tocs);
  return this;
}