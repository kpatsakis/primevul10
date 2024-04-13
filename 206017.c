Item_splocal::this_item() const
{
  DBUG_ASSERT(m_sp == m_thd->spcont->sp);

  return m_thd->spcont->get_item(m_var_idx);
}