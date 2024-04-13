void Item_direct_ref::save_val(Field *to)
{
  (*ref)->save_val(to);
  null_value=(*ref)->null_value;
}