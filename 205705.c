bool Item_ref::is_null()
{
  DBUG_ASSERT(fixed);
  bool tmp=(*ref)->is_null_result();
  null_value=(*ref)->null_value;
  return tmp;
}