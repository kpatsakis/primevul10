Item_cache* Item_cache::get_cache(const Item *item, const Item_result type)
{
  switch (type) {
  case INT_RESULT:
    return new Item_cache_int(item->field_type());
  case REAL_RESULT:
    return new Item_cache_real();
  case DECIMAL_RESULT:
    return new Item_cache_decimal();
  case STRING_RESULT:
    return new Item_cache_str(item);
  case ROW_RESULT:
    return new Item_cache_row();
  case TIME_RESULT:
    return new Item_cache_temporal(item->field_type());
  case IMPOSSIBLE_RESULT:
    DBUG_ASSERT(0);
    break;
  }
  return 0;                                     // Impossible
}