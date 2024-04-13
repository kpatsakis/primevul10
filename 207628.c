select_handler *SELECT_LEX::find_select_handler(THD *thd)
{
  if (next_select())
      return 0;
  if (master_unit()->outer_select())
    return 0;
  for (TABLE_LIST *tbl= join->tables_list; tbl; tbl= tbl->next_global)
  {
    if (!tbl->table)
      continue;
    handlerton *ht= tbl->table->file->partition_ht();
    if (!ht->create_select)
      continue;
    select_handler *sh= ht->create_select(thd, this);
    return sh;
  }
  return 0;
}