bool Item_field::set_no_const_sub(uchar *arg)
{
  if (field->charset() != &my_charset_bin)
    no_const_subst=1;
  return FALSE;
}