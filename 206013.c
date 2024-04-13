void Hybrid_type_traits_decimal::set_zero(Hybrid_type *val) const
{
  my_decimal_set_zero(&val->dec_buf[0]);
  val->used_dec_buf_no= 0;
}