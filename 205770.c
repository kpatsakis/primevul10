Item_sp_variable::Item_sp_variable(char *sp_var_name_str,
                                   uint sp_var_name_length)
  :m_thd(0)
#ifndef DBUG_OFF
   , m_sp(0)
#endif
{
  m_name.str= sp_var_name_str;
  m_name.length= sp_var_name_length;
}