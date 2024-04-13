String *Item_ref::val_str(String* tmp)
{
  DBUG_ASSERT(fixed);
  tmp=(*ref)->str_result(tmp);
  null_value=(*ref)->null_value;
  return tmp;
}