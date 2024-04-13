int SELECT_LEX::period_setup_conds(THD *thd, TABLE_LIST *tables)
{
  DBUG_ENTER("SELECT_LEX::period_setup_conds");
  const bool update_conds= !skip_setup_conds(thd);

  Query_arena backup;
  Query_arena *arena= thd->activate_stmt_arena_if_needed(&backup);

  DBUG_ASSERT(!tables->next_local && tables->table);

  Item *result= NULL;
  for (TABLE_LIST *table= tables; table; table= table->next_local)
  {
    if (!table->table)
      continue;
    vers_select_conds_t &conds= table->period_conditions;
    if (!table->table->s->period.name.streq(conds.name))
    {
      my_error(ER_PERIOD_NOT_FOUND, MYF(0), conds.name.str);
      if (arena)
        thd->restore_active_arena(arena, &backup);
      DBUG_RETURN(-1);
    }

    if (update_conds)
    {
      conds.period= &table->table->s->period;
      result= and_items(thd, result,
                        period_get_condition(thd, table, this, &conds, true));
    }
  }
  if (update_conds)
    where= and_items(thd, where, result);

  if (arena)
    thd->restore_active_arena(arena, &backup);

  DBUG_RETURN(0);
}