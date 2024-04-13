int SELECT_LEX::vers_setup_conds(THD *thd, TABLE_LIST *tables)
{
  DBUG_ENTER("SELECT_LEX::vers_setup_conds");
  const bool update_conds= !skip_setup_conds(thd);

  if (!versioned_tables)
  {
    for (TABLE_LIST *table= tables; table; table= table->next_local)
    {
      if (table->table && table->table->versioned())
        versioned_tables++;
      else if (table->vers_conditions.is_set() &&
              (table->is_non_derived() || !table->vers_conditions.used))
      {
        my_error(ER_VERS_NOT_VERSIONED, MYF(0), table->alias.str);
        DBUG_RETURN(-1);
      }
    }
  }

  if (versioned_tables == 0)
    DBUG_RETURN(0);

  /* For prepared statements we create items on statement arena,
     because they must outlive execution phase for multiple executions. */
  Query_arena_stmt on_stmt_arena(thd);

  // find outer system_time
  SELECT_LEX *outer_slex= outer_select();
  TABLE_LIST* outer_table= NULL;

  if (outer_slex)
  {
    TABLE_LIST* derived= master_unit()->derived;
    // inner SELECT may not be a derived table (derived == NULL)
    while (derived && outer_slex && !derived->vers_conditions.is_set())
    {
      derived= outer_slex->master_unit()->derived;
      outer_slex= outer_slex->outer_select();
    }
    if (derived && outer_slex)
    {
      DBUG_ASSERT(derived->vers_conditions.is_set());
      outer_table= derived;
    }
  }

  bool is_select= false;
  bool use_sysvar= false;
  switch (thd->lex->sql_command)
  {
  case SQLCOM_SELECT:
    use_sysvar= true;
    /* fall through */
  case SQLCOM_CREATE_TABLE:
  case SQLCOM_INSERT_SELECT:
  case SQLCOM_REPLACE_SELECT:
  case SQLCOM_DELETE_MULTI:
  case SQLCOM_UPDATE_MULTI:
    is_select= true;
  default:
    break;
  }

  for (TABLE_LIST *table= tables; table; table= table->next_local)
  {
    if (!table->table || table->is_view() || !table->table->versioned())
      continue;

    vers_select_conds_t &vers_conditions= table->vers_conditions;

#ifdef WITH_PARTITION_STORAGE_ENGINE
      /*
        if the history is stored in partitions, then partitions
        themselves are not versioned
      */
      if (table->partition_names && table->table->part_info->vers_info)
      {
        /* If the history is stored in partitions, then partitions
            themselves are not versioned. */
        if (vers_conditions.was_set())
        {
          my_error(ER_VERS_QUERY_IN_PARTITION, MYF(0), table->alias.str);
          DBUG_RETURN(-1);
        }
        else if (!vers_conditions.is_set())
          vers_conditions.set_all();
      }
#endif

    if (outer_table && !vers_conditions.is_set())
    {
      // propagate system_time from nearest outer SELECT_LEX
      vers_conditions= outer_table->vers_conditions;
      outer_table->vers_conditions.used= true;
    }

    // propagate system_time from sysvar
    if (!vers_conditions.is_set() && use_sysvar)
    {
      if (vers_conditions.init_from_sysvar(thd))
        DBUG_RETURN(-1);
    }

    if (vers_conditions.is_set())
    {
      if (vers_conditions.was_set() &&
          table->lock_type > TL_READ_NO_INSERT &&
          !vers_conditions.delete_history)
      {
        my_error(ER_TABLE_NOT_LOCKED_FOR_WRITE, MYF(0), table->alias.str);
        DBUG_RETURN(-1);
      }

      if (vers_conditions.type == SYSTEM_TIME_ALL)
        continue;
    }

    bool timestamps_only= table->table->versioned(VERS_TIMESTAMP);

    if (vers_conditions.is_set() && vers_conditions.type != SYSTEM_TIME_HISTORY)
    {
      thd->where= "FOR SYSTEM_TIME";
      /* TODO: do resolve fix_length_and_dec(), fix_fields(). This requires
        storing vers_conditions as Item and make some magic related to
        vers_system_time_t/VERS_TRX_ID at stage of fix_fields()
        (this is large refactoring). */
      if (vers_conditions.resolve_units(thd))
        DBUG_RETURN(-1);
      if (timestamps_only && (vers_conditions.start.unit == VERS_TRX_ID ||
        vers_conditions.end.unit == VERS_TRX_ID))
      {
        my_error(ER_VERS_ENGINE_UNSUPPORTED, MYF(0), table->table_name.str);
        DBUG_RETURN(-1);
      }
    }

    if (update_conds)
    {
      vers_conditions.period = &table->table->s->vers;
      Item *cond= period_get_condition(thd, table, this, &vers_conditions,
                                      timestamps_only);
      if (is_select)
        table->on_expr= and_items(thd, table->on_expr, cond);
      else
      {
        if (join)
        {
          where= and_items(thd, join->conds, cond);
          join->conds= where;
        }
        else
          where= and_items(thd, where, cond);
        table->where= and_items(thd, table->where, cond);
      }

      table->vers_conditions.set_all();
    }
  } // for (table= tables; ...)

  DBUG_RETURN(0);
}