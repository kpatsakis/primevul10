static void convert_zerofill_number_to_string(Item **item, Field_num *field)
{
  char buff[MAX_FIELD_WIDTH],*pos;
  String tmp(buff,sizeof(buff), field->charset()), *res;

  res= (*item)->val_str(&tmp);
  if ((*item)->is_null())
    *item= new Item_null();
  else
  {
    field->prepend_zeros(res);
    pos= (char *) sql_strmake (res->ptr(), res->length());
    *item= new Item_string(pos, res->length(), field->charset());
  }
}