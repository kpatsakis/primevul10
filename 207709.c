Item* period_get_condition(THD *thd, TABLE_LIST *table, SELECT_LEX *select,
                              vers_select_conds_t *conds, bool timestamp)
{
  DBUG_ASSERT(table);
  DBUG_ASSERT(table->table);
#define newx new (thd->mem_root)
  TABLE_SHARE *share= table->table->s;
  const TABLE_SHARE::period_info_t *period= conds->period;

  const LEX_CSTRING &fstart= period->start_field(share)->field_name;
  const LEX_CSTRING &fend= period->end_field(share)->field_name;

  conds->field_start= newx Item_field(thd, &select->context,
                                      table->db.str, table->alias.str,
                                      thd->make_clex_string(fstart));
  conds->field_end=   newx Item_field(thd, &select->context,
                                      table->db.str, table->alias.str,
                                      thd->make_clex_string(fend));

  Item *cond1= NULL, *cond2= NULL, *cond3= NULL, *curr= NULL;
  if (timestamp)
  {
    MYSQL_TIME max_time;
    switch (conds->type)
    {
    case SYSTEM_TIME_UNSPECIFIED:
    case SYSTEM_TIME_HISTORY:
    {
      thd->variables.time_zone->gmt_sec_to_TIME(&max_time, TIMESTAMP_MAX_VALUE);
      max_time.second_part= TIME_MAX_SECOND_PART;
      Datetime dt(&max_time);
      curr= newx Item_datetime_literal(thd, &dt, TIME_SECOND_PART_DIGITS);
      if (conds->type == SYSTEM_TIME_UNSPECIFIED)
        cond1= newx Item_func_eq(thd, conds->field_end, curr);
      else
        cond1= newx Item_func_lt(thd, conds->field_end, curr);
      break;
    }
    case SYSTEM_TIME_AS_OF:
      cond1= newx Item_func_le(thd, conds->field_start, conds->start.item);
      cond2= newx Item_func_gt(thd, conds->field_end, conds->start.item);
      break;
    case SYSTEM_TIME_FROM_TO:
      cond1= newx Item_func_lt(thd, conds->field_start, conds->end.item);
      cond2= newx Item_func_gt(thd, conds->field_end, conds->start.item);
      cond3= newx Item_func_lt(thd, conds->start.item, conds->end.item);
      break;
    case SYSTEM_TIME_BETWEEN:
      cond1= newx Item_func_le(thd, conds->field_start, conds->end.item);
      cond2= newx Item_func_gt(thd, conds->field_end, conds->start.item);
      cond3= newx Item_func_le(thd, conds->start.item, conds->end.item);
      break;
    case SYSTEM_TIME_BEFORE:
      cond1= newx Item_func_history(thd, conds->field_end);
      cond2= newx Item_func_lt(thd, conds->field_end, conds->start.item);
      break;
    default:
      DBUG_ASSERT(0);
    }
  }
  else
  {
    DBUG_ASSERT(table->table->s && table->table->s->db_plugin);

    Item *trx_id0= conds->start.item;
    Item *trx_id1= conds->end.item;
    if (conds->start.item && conds->start.unit == VERS_TIMESTAMP)
    {
      bool backwards= conds->type != SYSTEM_TIME_AS_OF;
      trx_id0= newx Item_func_trt_id(thd, conds->start.item,
                                     TR_table::FLD_TRX_ID, backwards);
    }
    if (conds->end.item && conds->end.unit == VERS_TIMESTAMP)
    {
      trx_id1= newx Item_func_trt_id(thd, conds->end.item,
                                     TR_table::FLD_TRX_ID, false);
    }

    switch (conds->type)
    {
    case SYSTEM_TIME_UNSPECIFIED:
    case SYSTEM_TIME_HISTORY:
      curr= newx Item_int(thd, ULONGLONG_MAX);
      if (conds->type == SYSTEM_TIME_UNSPECIFIED)
        cond1= newx Item_func_eq(thd, conds->field_end, curr);
      else
        cond1= newx Item_func_lt(thd, conds->field_end, curr);
      break;
      DBUG_ASSERT(!conds->start.item);
      DBUG_ASSERT(!conds->end.item);
      break;
    case SYSTEM_TIME_AS_OF:
      cond1= newx Item_func_trt_trx_sees_eq(thd, trx_id0, conds->field_start);
      cond2= newx Item_func_trt_trx_sees(thd, conds->field_end, trx_id0);
      DBUG_ASSERT(!conds->end.item);
      break;
    case SYSTEM_TIME_FROM_TO:
      cond1= newx Item_func_trt_trx_sees(thd, trx_id1, conds->field_start);
      cond2= newx Item_func_trt_trx_sees_eq(thd, conds->field_end, trx_id0);
      cond3= newx Item_func_lt(thd, conds->start.item, conds->end.item);
      break;
    case SYSTEM_TIME_BETWEEN:
      cond1= newx Item_func_trt_trx_sees_eq(thd, trx_id1, conds->field_start);
      cond2= newx Item_func_trt_trx_sees_eq(thd, conds->field_end, trx_id0);
      cond3= newx Item_func_le(thd, conds->start.item, conds->end.item);
      break;
    case SYSTEM_TIME_BEFORE:
      cond1= newx Item_func_history(thd, conds->field_end);
      cond2= newx Item_func_trt_trx_sees(thd, trx_id0, conds->field_end);
      break;
    default:
      DBUG_ASSERT(0);
    }
  }

  if (cond1)
  {
    cond1= and_items(thd, cond2, cond1);
    cond1= and_items(thd, cond3, cond1);
  }
  return cond1;
}