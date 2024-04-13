uint Item::decimal_precision() const
{
  Item_result restype= result_type();

  if ((restype == DECIMAL_RESULT) || (restype == INT_RESULT))
  {
    uint prec= 
      my_decimal_length_to_precision(max_char_length(), decimals,
                                     unsigned_flag);
    return min(prec, DECIMAL_MAX_PRECISION);
  }
  return min(max_char_length(), DECIMAL_MAX_PRECISION);
}