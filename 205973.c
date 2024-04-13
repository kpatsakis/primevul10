void Item_param::set_decimal(const char *str, ulong length)
{
  char *end;
  DBUG_ENTER("Item_param::set_decimal");

  end= (char*) str+length;
  str2my_decimal(E_DEC_FATAL_ERROR, str, &decimal_value, &end);
  state= DECIMAL_VALUE;
  decimals= decimal_value.frac;
  max_length=
    my_decimal_precision_to_length_no_truncation(decimal_value.precision(),
                                                 decimals, unsigned_flag);
  maybe_null= 0;
  DBUG_VOID_RETURN;
}