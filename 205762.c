Item_copy *Item_copy::create (Item *item)
{
  switch (item->result_type())
  {
    case STRING_RESULT:
      return new Item_copy_string (item);
    case REAL_RESULT: 
      return new Item_copy_float (item);
    case INT_RESULT:
      return item->unsigned_flag ? 
        new Item_copy_uint (item) : new Item_copy_int (item);
    case DECIMAL_RESULT:
      return new Item_copy_decimal (item);
    case TIME_RESULT:
    case ROW_RESULT:
  case IMPOSSIBLE_RESULT:
      DBUG_ASSERT (0);
  }
  /* should not happen */
  return NULL;
}