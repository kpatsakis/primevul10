uint32 Item_type_holder::display_length(Item *item)
{
  if (item->type() == Item::FIELD_ITEM)
    return ((Item_field *)item)->max_disp_length();

  switch (item->field_type())
  {
  case MYSQL_TYPE_DECIMAL:
  case MYSQL_TYPE_TIMESTAMP:
  case MYSQL_TYPE_DATE:
  case MYSQL_TYPE_TIME:
  case MYSQL_TYPE_DATETIME:
  case MYSQL_TYPE_YEAR:
  case MYSQL_TYPE_NEWDATE:
  case MYSQL_TYPE_VARCHAR:
  case MYSQL_TYPE_BIT:
  case MYSQL_TYPE_NEWDECIMAL:
  case MYSQL_TYPE_ENUM:
  case MYSQL_TYPE_SET:
  case MYSQL_TYPE_TINY_BLOB:
  case MYSQL_TYPE_MEDIUM_BLOB:
  case MYSQL_TYPE_LONG_BLOB:
  case MYSQL_TYPE_BLOB:
  case MYSQL_TYPE_VAR_STRING:
  case MYSQL_TYPE_STRING:
  case MYSQL_TYPE_GEOMETRY:
    return item->max_length;
  case MYSQL_TYPE_TINY:
    return 4;
  case MYSQL_TYPE_SHORT:
    return 6;
  case MYSQL_TYPE_LONG:
    return MY_INT32_NUM_DECIMAL_DIGITS;
  case MYSQL_TYPE_FLOAT:
    return 25;
  case MYSQL_TYPE_DOUBLE:
    return 53;
  case MYSQL_TYPE_NULL:
    return 0;
  case MYSQL_TYPE_LONGLONG:
    return 20;
  case MYSQL_TYPE_INT24:
    return 8;
  default:
    DBUG_ASSERT(0); // we should never go there
    return 0;
  }
}