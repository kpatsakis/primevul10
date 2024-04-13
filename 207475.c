commit_one_phase_2(THD *thd, bool all, THD_TRANS *trans, bool is_real_trans)
{
  int error= 0;
  uint count= 0;
  Ha_trx_info *ha_info= trans->ha_list, *ha_info_next;
  DBUG_ENTER("commit_one_phase_2");
  if (is_real_trans)
    DEBUG_SYNC(thd, "commit_one_phase_2");

  if (ha_info)
  {
    for (; ha_info; ha_info= ha_info_next)
    {
      int err;
      handlerton *ht= ha_info->ht();
      if ((err= ht->commit(ht, thd, all)))
      {
        my_error(ER_ERROR_DURING_COMMIT, MYF(0), err);
        error=1;
      }
      /* Should this be done only if is_real_trans is set ? */
      status_var_increment(thd->status_var.ha_commit_count);
      if (is_real_trans && ht != binlog_hton && ha_info->is_trx_read_write())
        ++count;
      ha_info_next= ha_info->next();
      ha_info->reset(); /* keep it conveniently zero-filled */
    }
    trans->ha_list= 0;
    trans->no_2pc=0;
    if (all)
    {
#ifdef HAVE_QUERY_CACHE
      if (thd->transaction.changed_tables)
        query_cache.invalidate(thd, thd->transaction.changed_tables);
#endif
    }
  }

  /* Free resources and perform other cleanup even for 'empty' transactions. */
  if (is_real_trans)
  {
    thd->has_waiter= false;
    thd->transaction.cleanup();
    if (count >= 2)
      statistic_increment(transactions_multi_engine, LOCK_status);
  }

  DBUG_RETURN(error);
}