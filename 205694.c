bool is_outer_table(TABLE_LIST *table, SELECT_LEX *select)
{
  DBUG_ASSERT(table->select_lex != select);
  TABLE_LIST *tl;

  if (table->belong_to_view &&
      table->belong_to_view->select_lex == select)
    return FALSE;

  for (tl= select->master_unit()->derived;
       tl && tl->is_merged_derived();
       select= tl->select_lex, tl= select->master_unit()->derived)
  {
    if (tl->select_lex == table->select_lex)
      return FALSE;
  }
  return TRUE;
}