Hybrid_type_traits::val_str(Hybrid_type *val, String *to, uint8 decimals) const
{
  to->set_real(val->real, decimals, &my_charset_bin);
  return to;
}