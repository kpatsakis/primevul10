void Item_param::make_field(Send_field *field)
{
  Item::make_field(field);

  if (!m_out_param_info)
    return;

  /*
    This is an OUT-parameter of stored procedure. We should use
    OUT-parameter info to fill out the names.
  */

  field->db_name= m_out_param_info->db_name;
  field->table_name= m_out_param_info->table_name;
  field->org_table_name= m_out_param_info->org_table_name;
  field->col_name= m_out_param_info->col_name;
  field->org_col_name= m_out_param_info->org_col_name;

  field->length= m_out_param_info->length;
  field->charsetnr= m_out_param_info->charsetnr;
  field->flags= m_out_param_info->flags;
  field->decimals= m_out_param_info->decimals;
  field->type= m_out_param_info->type;
}