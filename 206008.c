bool Item_ref::is_null_result()
{
  if (result_field)
    return (null_value=result_field->is_null());

  return is_null();
}