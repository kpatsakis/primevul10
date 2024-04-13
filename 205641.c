Item_case_expr::Item_case_expr(uint case_expr_id)
  :Item_sp_variable( C_STRING_WITH_LEN("case_expr")),
   m_case_expr_id(case_expr_id)
{
}