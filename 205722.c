int Item_cache_int::save_in_field(Field *field, bool no_conversions)
{
  int error;
  if (!has_value())
    return set_field_to_null_with_conversions(field, no_conversions);

  field->set_notnull();
  error= field->store(value, unsigned_flag);

  return error ? error : field->table->in_use->is_error() ? 1 : 0;
}