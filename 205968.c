Hybrid_type_traits_decimal::val_real(Hybrid_type *val) const
{
  my_decimal2double(E_DEC_FATAL_ERROR, &val->dec_buf[val->used_dec_buf_no],
                    &val->real);
  return val->real;
}