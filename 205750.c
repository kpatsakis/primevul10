Item *Item_static_float_func::safe_charset_converter(CHARSET_INFO *tocs)
{
  Item_string *conv;
  char buf[64];
  String *s, tmp(buf, sizeof(buf), &my_charset_bin);
  s= val_str(&tmp);
  if ((conv= new Item_static_string_func(func_name, s->ptr(), s->length(),
                                         s->charset())))
  {
    conv->str_value.copy();
    conv->str_value.mark_as_const();
  }
  return conv;
}