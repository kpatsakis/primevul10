void Item_field::save_result(Field *to)
{
  save_field_in_field(result_field, &null_value, to, TRUE);
}