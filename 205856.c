Item_splocal::this_item_addr(THD *thd, Item **)
{
  DBUG_ASSERT(m_sp == thd->spcont->sp);

  return thd->spcont->get_item_addr(m_var_idx);
}