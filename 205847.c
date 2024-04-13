Hybrid_type_traits_integer::fix_length_and_dec(Item *item, Item *arg) const
{
  item->decimals= 0;
  item->max_length= MY_INT64_NUM_DECIMAL_DIGITS;
  item->unsigned_flag= 0;
}