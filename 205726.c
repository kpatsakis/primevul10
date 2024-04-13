Item_case_expr::this_item()
{
  DBUG_ASSERT(m_sp == m_thd->spcont->sp);

  return m_thd->spcont->get_case_expr(m_case_expr_id);
}