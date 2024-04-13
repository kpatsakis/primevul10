Hybrid_type_traits_decimal::fix_length_and_dec(Item *item, Item *arg) const
{
  item->decimals= arg->decimals;
  item->max_length= min(arg->max_length + DECIMAL_LONGLONG_DIGITS,
                        DECIMAL_MAX_STR_LENGTH);
}