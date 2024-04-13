static int save_int_value_in_field (Field *field, longlong nr, 
                                    bool null_value, bool unsigned_flag)
{
  if (null_value)
    return set_field_to_null(field);
  field->set_notnull();
  return field->store(nr, unsigned_flag);
}