void Item_param::set_decimal(const my_decimal *dv)
{
  state= DECIMAL_VALUE;

  my_decimal2decimal(dv, &decimal_value);

  decimals= (uint8) decimal_value.frac;
  unsigned_flag= !decimal_value.sign();
  max_length= my_decimal_precision_to_length(decimal_value.intg + decimals,
                                             decimals, unsigned_flag);
}