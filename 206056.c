Item_splocal::Item_splocal(const LEX_STRING &sp_var_name,
                           uint sp_var_idx,
                           enum_field_types sp_var_type,
                           uint pos_in_q, uint len_in_q)
  :Item_sp_variable(sp_var_name.str, sp_var_name.length),
   m_var_idx(sp_var_idx),
   limit_clause_param(FALSE),
   pos_in_query(pos_in_q), len_in_query(len_in_q)
{
  maybe_null= TRUE;

  m_type= sp_map_item_type(sp_var_type);
  m_field_type= sp_var_type;
  m_result_type= sp_map_result_type(sp_var_type);
}