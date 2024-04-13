void Hybrid_type_traits_decimal::div(Hybrid_type *val, ulonglong u) const
{
  int2my_decimal(E_DEC_FATAL_ERROR, u, TRUE, &val->dec_buf[2]);
  /* XXX: what is '4' for scale? */
  my_decimal_div(E_DEC_FATAL_ERROR,
                 &val->dec_buf[val->used_dec_buf_no ^ 1],
                 &val->dec_buf[val->used_dec_buf_no],
                 &val->dec_buf[2], 4);
  val->used_dec_buf_no^= 1;
}