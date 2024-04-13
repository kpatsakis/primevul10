enum_field_types Item::string_field_type() const
{
  enum_field_types f_type= MYSQL_TYPE_VAR_STRING;
  if (max_length >= 16777216)
    f_type= MYSQL_TYPE_LONG_BLOB;
  else if (max_length >= 65536)
    f_type= MYSQL_TYPE_MEDIUM_BLOB;
  return f_type;
}