Item_case_expr::this_item_addr(THD *thd, Item **)
{
  DBUG_ASSERT(m_sp == thd->spcont->sp);

  return thd->spcont->get_case_expr_addr(m_case_expr_id);
}