int Item_int::save_in_field(Field *field, bool no_conversions)
{
  return save_int_value_in_field (field, val_int(), null_value, unsigned_flag);
}