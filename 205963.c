void Item_ref::save_result(Field *to)
{
  if (result_field)
  {
    save_field_in_field(result_field, &null_value, to, TRUE);
    return;
  }
  (*ref)->save_result(to);
  null_value= (*ref)->null_value;
}