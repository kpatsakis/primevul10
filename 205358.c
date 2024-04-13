void run_query(struct st_connection *cn, struct st_command *command, int flags)
{
  MYSQL *mysql= cn->mysql;
  DYNAMIC_STRING *ds;
  DYNAMIC_STRING *save_ds= NULL;
  DYNAMIC_STRING ds_result;
  DYNAMIC_STRING ds_sorted;
  DYNAMIC_STRING ds_warnings;
  char *query;
  int query_len;
  my_bool view_created= 0, sp_created= 0;
  my_bool complete_query= ((flags & QUERY_SEND_FLAG) &&
                           (flags & QUERY_REAP_FLAG));
  DBUG_ENTER("run_query");

  if (cn->pending && (flags & QUERY_SEND_FLAG))
    die("Cannot run query on connection between send and reap");

  if (!(flags & QUERY_SEND_FLAG) && !cn->pending)
    die("Cannot reap on a connection without pending send");
  
  init_dynamic_string(&ds_warnings, NULL, 0, 256);
  ds_warn= &ds_warnings;
  
  /*
    Evaluate query if this is an eval command
  */
  if (command->type == Q_EVAL || command->type == Q_SEND_EVAL)
  {
    if (!command->eval_query.str)
      init_dynamic_string(&command->eval_query, "", command->query_len + 256,
                          1024);
    else
      dynstr_set(&command->eval_query, 0);
    do_eval(&command->eval_query, command->query, command->end, FALSE);
    query= command->eval_query.str;
    query_len= command->eval_query.length;
  }
  else
  {
    query = command->query;
    query_len = strlen(query);
  }

  /*
    When command->require_file is set the output of _this_ query
    should be compared with an already existing file
    Create a temporary dynamic string to contain the output from
    this query.
  */
  if (command->require_file)
  {
    init_dynamic_string(&ds_result, "", 1024, 1024);
    ds= &ds_result;
  }
  else
    ds= &ds_res;

  /*
    Log the query into the output buffer
  */
  if (!disable_query_log && (flags & QUERY_SEND_FLAG))
  {
    replace_dynstr_append_mem(ds, query, query_len);
    dynstr_append_mem(ds, delimiter, delimiter_length);
    dynstr_append_mem(ds, "\n", 1);
  }

  /*
    Write the command to the result file before we execute the query
    This is needed to be able to analyse the log if something goes
    wrong
  */
  log_file.write(&ds_res);
  log_file.flush();
  dynstr_set(&ds_res, 0);

  if (view_protocol_enabled &&
      complete_query &&
      match_re(&view_re, query))
  {
    /*
      Create the query as a view.
      Use replace since view can exist from a failed mysqltest run
    */
    DYNAMIC_STRING query_str;
    init_dynamic_string(&query_str,
			"CREATE OR REPLACE VIEW mysqltest_tmp_v AS ",
			query_len+64, 256);
    dynstr_append_mem(&query_str, query, query_len);
    if (util_query(mysql, query_str.str))
    {
      /*
	Failed to create the view, this is not fatal
	just run the query the normal way
      */
      DBUG_PRINT("view_create_error",
		 ("Failed to create view '%s': %d: %s", query_str.str,
		  mysql_errno(mysql), mysql_error(mysql)));

      /* Log error to create view */
      verbose_msg("Failed to create view '%s' %d: %s", query_str.str,
		  mysql_errno(mysql), mysql_error(mysql));
    }
    else
    {
      /*
	Yes, it was possible to create this query as a view
      */
      view_created= 1;
      query= (char*)"SELECT * FROM mysqltest_tmp_v";
      query_len = strlen(query);

      /*
        Collect warnings from create of the view that should otherwise
        have been produced when the SELECT was executed
      */
      append_warnings(&ds_warnings, cur_con->util_mysql);
    }

    dynstr_free(&query_str);
  }

  if (sp_protocol_enabled &&
      complete_query &&
      match_re(&sp_re, query))
  {
    /*
      Create the query as a stored procedure
      Drop first since sp can exist from a failed mysqltest run
    */
    DYNAMIC_STRING query_str;
    init_dynamic_string(&query_str,
			"DROP PROCEDURE IF EXISTS mysqltest_tmp_sp;",
			query_len+64, 256);
    util_query(mysql, query_str.str);
    dynstr_set(&query_str, "CREATE PROCEDURE mysqltest_tmp_sp()\n");
    dynstr_append_mem(&query_str, query, query_len);
    if (util_query(mysql, query_str.str))
    {
      /*
	Failed to create the stored procedure for this query,
	this is not fatal just run the query the normal way
      */
      DBUG_PRINT("sp_create_error",
		 ("Failed to create sp '%s': %d: %s", query_str.str,
		  mysql_errno(mysql), mysql_error(mysql)));

      /* Log error to create sp */
      verbose_msg("Failed to create sp '%s' %d: %s", query_str.str,
		  mysql_errno(mysql), mysql_error(mysql));

    }
    else
    {
      sp_created= 1;

      query= (char*)"CALL mysqltest_tmp_sp()";
      query_len = strlen(query);
    }
    dynstr_free(&query_str);
  }

  if (display_result_sorted)
  {
    /*
       Collect the query output in a separate string
       that can be sorted before it's added to the
       global result string
    */
    init_dynamic_string(&ds_sorted, "", 1024, 1024);
    save_ds= ds; /* Remember original ds */
    ds= &ds_sorted;
  }

  /*
    Find out how to run this query

    Always run with normal C API if it's not a complete
    SEND + REAP

    If it is a '?' in the query it may be a SQL level prepared
    statement already and we can't do it twice
  */
  if (ps_protocol_enabled &&
      complete_query &&
      match_re(&ps_re, query))
    run_query_stmt(cn, command, query, query_len, ds, &ds_warnings);
  else
    run_query_normal(cn, command, flags, query, query_len,
		     ds, &ds_warnings);

  dynstr_free(&ds_warnings);
  ds_warn= 0;

  if (display_result_sorted)
  {
    /* Sort the result set and append it to result */
    dynstr_append_sorted(save_ds, &ds_sorted, 1);
    ds= save_ds;
    dynstr_free(&ds_sorted);
  }

  if (sp_created)
  {
    if (util_query(mysql, "DROP PROCEDURE mysqltest_tmp_sp "))
      report_or_die("Failed to drop sp: %d: %s", mysql_errno(mysql),
                     mysql_error(mysql));
  }

  if (view_created)
  {
    if (util_query(mysql, "DROP VIEW mysqltest_tmp_v "))
      report_or_die("Failed to drop view: %d: %s",
	  mysql_errno(mysql), mysql_error(mysql));
  }

  if (command->require_file)
  {
    /* A result file was specified for _this_ query
       and the output should be checked against an already
       existing file which has been specified using --require or --result
    */
    check_require(ds, command->require_file);
  }

  if (ds == &ds_result)
    dynstr_free(&ds_result);
  DBUG_VOID_RETURN;
}