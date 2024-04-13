int Item_ref::save_in_field(Field *to, bool no_conversions)
{
  int res;
  if (result_field)
  {
    if (result_field->is_null())
    {
      null_value= 1;
      res= set_field_to_null_with_conversions(to, no_conversions);
      return res;
    }
    to->set_notnull();
    res= field_conv(to, result_field);
    null_value= 0;
    return res;
  }
  res= (*ref)->save_in_field(to, no_conversions);
  null_value= (*ref)->null_value;
  return res;
}