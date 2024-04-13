Item *Item_num::safe_charset_converter(CHARSET_INFO *tocs)
{
  /*
    Item_num returns pure ASCII result,
    so conversion is needed only in case of "tricky" character
    sets like UCS2. If tocs is not "tricky", return the item itself.
  */
  if (!(tocs->state & MY_CS_NONASCII))
    return this;
  
  Item_string *conv;
  uint conv_errors;
  char buf[64], buf2[64];
  String tmp(buf, sizeof(buf), &my_charset_bin);
  String cstr(buf2, sizeof(buf2), &my_charset_bin);
  String *ostr= val_str(&tmp);
  char *ptr;
  cstr.copy(ostr->ptr(), ostr->length(), ostr->charset(), tocs, &conv_errors);
  if (conv_errors || !(conv= new Item_string(cstr.ptr(), cstr.length(),
                                             cstr.charset(),
                                             collation.derivation)))
  {
    /*
      Safe conversion is not possible (or EOM).
      We could not convert a string into the requested character set
      without data loss. The target charset does not cover all the
      characters from the string. Operation cannot be done correctly.
    */
    return NULL;
  }
  if (!(ptr= current_thd->strmake(cstr.ptr(), cstr.length())))
    return NULL;
  conv->str_value.set(ptr, cstr.length(), cstr.charset());
  /* Ensure that no one is going to change the result string */
  conv->str_value.mark_as_const();
  conv->fix_char_length(max_char_length());
  return conv;
}