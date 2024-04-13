bool Item_direct_ref::is_null()
{
  return (*ref)->is_null();
}