longlong Item_field::val_int_result()
{
  if ((null_value=result_field->is_null()))
    return 0;
  return result_field->val_int();
}