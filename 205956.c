Hybrid_type_traits_decimal::val_str(Hybrid_type *val, String *to,
                                    uint8 decimals) const
{
  my_decimal_round(E_DEC_FATAL_ERROR, &val->dec_buf[val->used_dec_buf_no],
                   decimals, FALSE, &val->dec_buf[2]);
  my_decimal2string(E_DEC_FATAL_ERROR, &val->dec_buf[2], 0, 0, 0, to);
  return to;
}