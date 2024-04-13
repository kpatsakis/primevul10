void Item::set_name_no_truncate(const char *str, uint length, CHARSET_INFO *cs)
{
  if (!my_charset_same(cs, system_charset_info))
  {
    size_t res_length;
    name= sql_strmake_with_convert(str, length, cs,
				   UINT_MAX, system_charset_info,
				   &res_length);
    name_length= res_length;
  }
  else
    name= sql_strmake(str, (name_length= length));
}