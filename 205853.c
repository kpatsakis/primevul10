bool Item_field::is_null_result()
{
  return (null_value=result_field->is_null());
}