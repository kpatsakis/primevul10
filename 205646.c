Item_param::set_out_param_info(Send_field *info)
{
  m_out_param_info= info;
  param_type= m_out_param_info->type;
}