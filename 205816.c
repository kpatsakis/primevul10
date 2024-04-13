enum_field_types Item::field_type() const
{
  switch (result_type()) {
  case STRING_RESULT:  return string_field_type();
  case INT_RESULT:     return MYSQL_TYPE_LONGLONG;
  case DECIMAL_RESULT: return MYSQL_TYPE_NEWDECIMAL;
  case REAL_RESULT:    return MYSQL_TYPE_DOUBLE;
  case ROW_RESULT:
  case TIME_RESULT:
  case IMPOSSIBLE_RESULT:
    DBUG_ASSERT(0);
    return MYSQL_TYPE_VARCHAR;
  }
  return MYSQL_TYPE_VARCHAR;
}