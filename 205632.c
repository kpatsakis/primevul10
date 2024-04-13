void Item_ref::save_val(Field *to)
{
  (*ref)->save_result(to);
  null_value= (*ref)->null_value;
}