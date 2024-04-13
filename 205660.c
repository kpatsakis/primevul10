void Item_case_expr::print(String *str, enum_query_type)
{
  if (str->reserve(MAX_INT_WIDTH + sizeof("case_expr@")))
    return;                                    /* purecov: inspected */
  (void) str->append(STRING_WITH_LEN("case_expr@"));
  str->qs_append(m_case_expr_id);
}