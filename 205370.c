void run_query_stmt(struct st_connection *cn, struct st_command *command,
                    char *query, int query_len, DYNAMIC_STRING *ds,
                    DYNAMIC_STRING *ds_warnings)
{
  MYSQL_RES *res= NULL;     /* Note that here 'res' is meta data result set */
  MYSQL *mysql= cn->mysql;
  MYSQL_STMT *stmt;
  DYNAMIC_STRING ds_prepare_warnings;
  DYNAMIC_STRING ds_execute_warnings;
  DBUG_ENTER("run_query_stmt");
  DBUG_PRINT("query", ("'%-.60s'", query));

  /*
    Init a new stmt if it's not already one created for this connection
  */
  if(!(stmt= cn->stmt))
  {
    if (!(stmt= mysql_stmt_init(mysql)))
      die("unable to init stmt structure");
    cn->stmt= stmt;
  }

  /* Init dynamic strings for warnings */
  if (!disable_warnings)
  {
    init_dynamic_string(&ds_prepare_warnings, NULL, 0, 256);
    init_dynamic_string(&ds_execute_warnings, NULL, 0, 256);
  }

  /*
    Prepare the query
  */
  if (do_stmt_prepare(cn, query, query_len))
  {
    handle_error(command,  mysql_stmt_errno(stmt),
                 mysql_stmt_error(stmt), mysql_stmt_sqlstate(stmt), ds);
    goto end;
  }

  /*
    Get the warnings from mysql_stmt_prepare and keep them in a
    separate string
  */
  if (!disable_warnings)
    append_warnings(&ds_prepare_warnings, mysql);

  /*
    No need to call mysql_stmt_bind_param() because we have no
    parameter markers.
  */

#if MYSQL_VERSION_ID >= 50000
  if (cursor_protocol_enabled)
  {
    /*
      Use cursor when retrieving result
    */
    ulong type= CURSOR_TYPE_READ_ONLY;
    if (mysql_stmt_attr_set(stmt, STMT_ATTR_CURSOR_TYPE, (void*) &type))
      die("mysql_stmt_attr_set(STMT_ATTR_CURSOR_TYPE) failed': %d %s",
          mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
  }
#endif

  /*
    Execute the query
  */
  if (do_stmt_execute(cn))
  {
    handle_error(command, mysql_stmt_errno(stmt),
                 mysql_stmt_error(stmt), mysql_stmt_sqlstate(stmt), ds);
    goto end;
  }

  /*
    When running in cursor_protocol get the warnings from execute here
    and keep them in a separate string for later.
  */
  if (cursor_protocol_enabled && !disable_warnings)
    append_warnings(&ds_execute_warnings, mysql);

  /*
    We instruct that we want to update the "max_length" field in
    mysql_stmt_store_result(), this is our only way to know how much
    buffer to allocate for result data
  */
  {
    my_bool one= 1;
    if (mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, (void*) &one))
      die("mysql_stmt_attr_set(STMT_ATTR_UPDATE_MAX_LENGTH) failed': %d %s",
          mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
  }

  /*
    If we got here the statement succeeded and was expected to do so,
    get data. Note that this can still give errors found during execution!
    Store the result of the query if if will return any fields
  */
  if (mysql_stmt_field_count(stmt) && mysql_stmt_store_result(stmt))
  {
    handle_error(command, mysql_stmt_errno(stmt),
                 mysql_stmt_error(stmt), mysql_stmt_sqlstate(stmt), ds);
    goto end;
  }

  /* If we got here the statement was both executed and read successfully */
  handle_no_error(command);
  if (!disable_result_log)
  {
    /*
      Not all statements creates a result set. If there is one we can
      now create another normal result set that contains the meta
      data. This set can be handled almost like any other non prepared
      statement result set.
    */
    if ((res= mysql_stmt_result_metadata(stmt)) != NULL)
    {
      /* Take the column count from meta info */
      MYSQL_FIELD *fields= mysql_fetch_fields(res);
      uint num_fields= mysql_num_fields(res);

      if (display_metadata)
        append_metadata(ds, fields, num_fields);

      if (!display_result_vertically)
        append_table_headings(ds, fields, num_fields);

      append_stmt_result(ds, stmt, fields, num_fields);

      mysql_free_result(res);     /* Free normal result set with meta data */

      /*
        Normally, if there is a result set, we do not show warnings from the
        prepare phase. This is because some warnings are generated both during
        prepare and execute; this would generate different warning output
        between normal and ps-protocol test runs.

        The --enable_prepare_warnings command can be used to change this so
        that warnings from both the prepare and execute phase are shown.
      */
      if (!disable_warnings && !prepare_warnings_enabled)
        dynstr_set(&ds_prepare_warnings, NULL);
    }
    else
    {
      /*
	This is a query without resultset
      */
    }

    /*
      Fetch info before fetching warnings, since it will be reset
      otherwise.
    */
    if (!disable_info)
      append_info(ds, mysql_stmt_affected_rows(stmt), mysql_info(mysql));

    if (!disable_warnings)
    {
      /* Get the warnings from execute */

      /* Append warnings to ds - if there are any */
      if (append_warnings(&ds_execute_warnings, mysql) ||
          ds_execute_warnings.length ||
          ds_prepare_warnings.length ||
          ds_warnings->length)
      {
        dynstr_append_mem(ds, "Warnings:\n", 10);
        if (ds_warnings->length)
          dynstr_append_mem(ds, ds_warnings->str,
                            ds_warnings->length);
        if (ds_prepare_warnings.length)
          dynstr_append_mem(ds, ds_prepare_warnings.str,
                            ds_prepare_warnings.length);
        if (ds_execute_warnings.length)
          dynstr_append_mem(ds, ds_execute_warnings.str,
                            ds_execute_warnings.length);
      }
    }
  }

end:
  if (!disable_warnings)
  {
    dynstr_free(&ds_prepare_warnings);
    dynstr_free(&ds_execute_warnings);
  }

  /*
    We save the return code (mysql_stmt_errno(stmt)) from the last call sent
    to the server into the mysqltest builtin variable $mysql_errno. This
    variable then can be used from the test case itself.
  */

  var_set_errno(mysql_stmt_errno(stmt));

  revert_properties();

  /* Close the statement if reconnect, need new prepare */
  if (mysql->reconnect)
  {
    mysql_stmt_close(stmt);
    cn->stmt= NULL;
  }

  DBUG_VOID_RETURN;
}