Hybrid_type_traits_decimal::val_int(Hybrid_type *val, bool unsigned_flag) const
{
  longlong result;
  my_decimal2int(E_DEC_FATAL_ERROR, &val->dec_buf[val->used_dec_buf_no],
                 unsigned_flag, &result);
  return result;
}