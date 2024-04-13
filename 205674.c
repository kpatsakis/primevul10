void Item_splocal::print(String *str, enum_query_type)
{
  str->reserve(m_name.length+8);
  str->append(m_name.str, m_name.length);
  str->append('@');
  str->qs_append(m_var_idx);
}