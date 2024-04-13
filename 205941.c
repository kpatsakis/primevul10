Item_decimal::Item_decimal(const char *str, const my_decimal *val_arg,
                           uint decimal_par, uint length)
{
  my_decimal2decimal(val_arg, &decimal_value);
  name= (char*) str;
  decimals= (uint8) decimal_par;
  max_length= length;
  fixed= 1;
}