bool vers_select_conds_t::init_from_sysvar(THD *thd)
{
  vers_asof_timestamp_t &in= thd->variables.vers_asof_timestamp;
  type= (vers_system_time_t) in.type;
  delete_history= false;
  start.unit= VERS_TIMESTAMP;
  if (type != SYSTEM_TIME_UNSPECIFIED && type != SYSTEM_TIME_ALL)
  {
    DBUG_ASSERT(type == SYSTEM_TIME_AS_OF);
    Datetime dt(in.unix_time, in.second_part, thd->variables.time_zone);

    start.item= new (thd->mem_root)
        Item_datetime_literal(thd, &dt, TIME_SECOND_PART_DIGITS);
    if (!start.item)
      return true;
  }
  else
    start.item= NULL;
  end.empty();
  return false;
}