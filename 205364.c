int do_save_master_pos()
{
  MYSQL_RES *res;
  MYSQL_ROW row;
  MYSQL *mysql = cur_con->mysql;
  const char *query;
  DBUG_ENTER("do_save_master_pos");

#ifdef HAVE_NDB_BINLOG
  /*
    Wait for ndb binlog to be up-to-date with all changes
    done on the local mysql server
  */
  {
    ulong have_ndbcluster;
    if (mysql_query(mysql, query= "show variables like 'have_ndbcluster'"))
      die("'%s' failed: %d %s", query,
          mysql_errno(mysql), mysql_error(mysql));
    if (!(res= mysql_store_result(mysql)))
      die("mysql_store_result() returned NULL for '%s'", query);
    if (!(row= mysql_fetch_row(res)))
      die("Query '%s' returned empty result", query);

    have_ndbcluster= strcmp("YES", row[1]) == 0;
    mysql_free_result(res);

    if (have_ndbcluster)
    {
      ulonglong start_epoch= 0, handled_epoch= 0,
	latest_epoch=0, latest_trans_epoch=0,
	latest_handled_binlog_epoch= 0, latest_received_binlog_epoch= 0,
	latest_applied_binlog_epoch= 0;
      int count= 0;
      int do_continue= 1;
      while (do_continue)
      {
        const char binlog[]= "binlog";
	const char latest_epoch_str[]=
          "latest_epoch=";
        const char latest_trans_epoch_str[]=
          "latest_trans_epoch=";
	const char latest_received_binlog_epoch_str[]=
	  "latest_received_binlog_epoch";
        const char latest_handled_binlog_epoch_str[]=
          "latest_handled_binlog_epoch=";
        const char latest_applied_binlog_epoch_str[]=
          "latest_applied_binlog_epoch=";
        if (count)
          my_sleep(100*1000); /* 100ms */
        if (mysql_query(mysql, query= "show engine ndb status"))
          die("failed in '%s': %d %s", query,
              mysql_errno(mysql), mysql_error(mysql));
        if (!(res= mysql_store_result(mysql)))
          die("mysql_store_result() returned NULL for '%s'", query);
        while ((row= mysql_fetch_row(res)))
        {
          if (strcmp(row[1], binlog) == 0)
          {
            const char *status= row[2];

	    /* latest_epoch */
	    while (*status && strncmp(status, latest_epoch_str,
				      sizeof(latest_epoch_str)-1))
	      status++;
	    if (*status)
            {
	      status+= sizeof(latest_epoch_str)-1;
	      latest_epoch= strtoull(status, (char**) 0, 10);
	    }
	    else
	      die("result does not contain '%s' in '%s'",
		  latest_epoch_str, query);
	    /* latest_trans_epoch */
	    while (*status && strncmp(status, latest_trans_epoch_str,
				      sizeof(latest_trans_epoch_str)-1))
	      status++;
	    if (*status)
	    {
	      status+= sizeof(latest_trans_epoch_str)-1;
	      latest_trans_epoch= strtoull(status, (char**) 0, 10);
	    }
	    else
	      die("result does not contain '%s' in '%s'",
		  latest_trans_epoch_str, query);
	    /* latest_received_binlog_epoch */
	    while (*status &&
		   strncmp(status, latest_received_binlog_epoch_str,
			   sizeof(latest_received_binlog_epoch_str)-1))
	      status++;
	    if (*status)
	    {
	      status+= sizeof(latest_received_binlog_epoch_str)-1;
	      latest_received_binlog_epoch= strtoull(status, (char**) 0, 10);
	    }
	    else
	      die("result does not contain '%s' in '%s'",
		  latest_received_binlog_epoch_str, query);
	    /* latest_handled_binlog */
	    while (*status &&
		   strncmp(status, latest_handled_binlog_epoch_str,
			   sizeof(latest_handled_binlog_epoch_str)-1))
	      status++;
	    if (*status)
	    {
	      status+= sizeof(latest_handled_binlog_epoch_str)-1;
	      latest_handled_binlog_epoch= strtoull(status, (char**) 0, 10);
	    }
	    else
	      die("result does not contain '%s' in '%s'",
		  latest_handled_binlog_epoch_str, query);
	    /* latest_applied_binlog_epoch */
	    while (*status &&
		   strncmp(status, latest_applied_binlog_epoch_str,
			   sizeof(latest_applied_binlog_epoch_str)-1))
	      status++;
	    if (*status)
	    {
	      status+= sizeof(latest_applied_binlog_epoch_str)-1;
	      latest_applied_binlog_epoch= strtoull(status, (char**) 0, 10);
	    }
	    else
	      die("result does not contain '%s' in '%s'",
		  latest_applied_binlog_epoch_str, query);
	    if (count == 0)
	      start_epoch= latest_trans_epoch;
	    break;
	  }
	}
	if (!row)
	  die("result does not contain '%s' in '%s'",
	      binlog, query);
	if (latest_handled_binlog_epoch > handled_epoch)
	  count= 0;
	handled_epoch= latest_handled_binlog_epoch;
	count++;
	if (latest_handled_binlog_epoch >= start_epoch)
          do_continue= 0;
        else if (count > 300) /* 30s */
	{
	  break;
        }
        mysql_free_result(res);
      }
    }
  }
#endif
  if (mysql_query(mysql, query= "show master status"))
    die("failed in 'show master status': %d %s",
	mysql_errno(mysql), mysql_error(mysql));

  if (!(res = mysql_store_result(mysql)))
    die("mysql_store_result() retuned NULL for '%s'", query);
  if (!(row = mysql_fetch_row(res)))
    die("empty result in show master status");
  strnmov(master_pos.file, row[0], sizeof(master_pos.file)-1);
  master_pos.pos = strtoul(row[1], (char**) 0, 10);
  mysql_free_result(res);
  DBUG_RETURN(0);
}