void Hybrid_type_traits_decimal::add(Hybrid_type *val, Field *f) const
{
  my_decimal_add(E_DEC_FATAL_ERROR,
                 &val->dec_buf[val->used_dec_buf_no ^ 1],
                 &val->dec_buf[val->used_dec_buf_no],
                 f->val_decimal(&val->dec_buf[2]));
  val->used_dec_buf_no^= 1;
}