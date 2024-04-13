Item *Item_hex_constant::safe_charset_converter(CHARSET_INFO *tocs)
{
  Item_string *conv;
  String tmp, *str= val_str(&tmp);

  if (!(conv= new Item_string(str->ptr(), str->length(), tocs)))
    return NULL;
  conv->str_value.copy();
  conv->str_value.mark_as_const();
  return conv;
}