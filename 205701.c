Item *Item_param::safe_charset_converter(CHARSET_INFO *tocs)
{
  if (const_item())
  {
    uint cnv_errors;
    String *ostr= val_str(&cnvstr);
    if (null_value)
    {
      Item_null *n= new Item_null();
      return n ? n->safe_charset_converter(tocs) : NULL;
    }
    cnvitem->str_value.copy(ostr->ptr(), ostr->length(),
                            ostr->charset(), tocs, &cnv_errors);
    if (cnv_errors)
       return NULL;
    cnvitem->str_value.mark_as_const();
    cnvitem->max_length= cnvitem->str_value.numchars() * tocs->mbmaxlen;
    return cnvitem;
  }
  return Item::safe_charset_converter(tocs);
}