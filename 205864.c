int Item_hex_hybrid::save_in_field(Field *field, bool no_conversions)
{
  field->set_notnull();
  if (field->result_type() == STRING_RESULT)
    return field->store(str_value.ptr(), str_value.length(), 
                        collation.collation);

  ulonglong nr;
  uint32 length= str_value.length();
  if (!length)
    return 1;

  if (length > 8)
  {
    nr= field->flags & UNSIGNED_FLAG ? ULONGLONG_MAX : LONGLONG_MAX;
    goto warn;
  }
  nr= (ulonglong) val_int();
  if ((length == 8) && !(field->flags & UNSIGNED_FLAG) && (nr > LONGLONG_MAX))
  {
    nr= LONGLONG_MAX;
    goto warn;
  }
  return field->store((longlong) nr, TRUE);  // Assume hex numbers are unsigned

warn:
  if (!field->store((longlong) nr, TRUE))
    field->set_warning(MYSQL_ERROR::WARN_LEVEL_WARN, ER_WARN_DATA_OUT_OF_RANGE,
                       1);
  return 1;
}