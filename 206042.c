Hybrid_type_traits::val_decimal(Hybrid_type *val, my_decimal *to) const
{
  double2my_decimal(E_DEC_FATAL_ERROR, val->real, val->dec_buf);
  return val->dec_buf;
}