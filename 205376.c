void var_set_query_get_value(struct st_command *command, VAR *var)
{
  long row_no;
  int col_no= -1;
  MYSQL_RES* res;
  MYSQL* mysql= cur_con->mysql;

  static DYNAMIC_STRING ds_query;
  static DYNAMIC_STRING ds_col;
  static DYNAMIC_STRING ds_row;
  const struct command_arg query_get_value_args[] = {
    {"query", ARG_STRING, TRUE, &ds_query, "Query to run"},
    {"column name", ARG_STRING, TRUE, &ds_col, "Name of column"},
    {"row number", ARG_STRING, TRUE, &ds_row, "Number for row"}
  };

  DBUG_ENTER("var_set_query_get_value");
  LINT_INIT(res);

  if (!mysql)
  {
    handle_no_active_connection(command, cur_con, &ds_res);
    DBUG_VOID_RETURN;
  }

  strip_parentheses(command);
  DBUG_PRINT("info", ("query: %s", command->query));
  check_command_args(command, command->first_argument, query_get_value_args,
                     sizeof(query_get_value_args)/sizeof(struct command_arg),
                     ',');

  DBUG_PRINT("info", ("query: %s", ds_query.str));
  DBUG_PRINT("info", ("col: %s", ds_col.str));

  /* Convert row number to int */
  if (!str2int(ds_row.str, 10, (long) 0, (long) INT_MAX, &row_no))
    die("Invalid row number: '%s'", ds_row.str);
  DBUG_PRINT("info", ("row: %s, row_no: %ld", ds_row.str, row_no));
  dynstr_free(&ds_row);

  /* Remove any surrounding "'s from the query - if there is any */
  if (strip_surrounding(ds_query.str, '"', '"'))
    die("Mismatched \"'s around query '%s'", ds_query.str);

  /* Run the query */
  if (mysql_real_query(mysql, ds_query.str, ds_query.length))
  {
    handle_error(curr_command, mysql_errno(mysql), mysql_error(mysql),
                 mysql_sqlstate(mysql), &ds_res);
    /* If error was acceptable, return empty string */
    dynstr_free(&ds_query);
    dynstr_free(&ds_col);
    eval_expr(var, "", 0);
    DBUG_VOID_RETURN;
  }

  if (!(res= mysql_store_result(mysql)))
  {
    report_or_die("Query '%s' didn't return a result set", ds_query.str);
    dynstr_free(&ds_query);
    dynstr_free(&ds_col);
    eval_expr(var, "", 0);
    return;
  }

  {
    /* Find column number from the given column name */
    uint i;
    uint num_fields= mysql_num_fields(res);
    MYSQL_FIELD *fields= mysql_fetch_fields(res);

    for (i= 0; i < num_fields; i++)
    {
      if (strcmp(fields[i].name, ds_col.str) == 0 &&
          strlen(fields[i].name) == ds_col.length)
      {
        col_no= i;
        break;
      }
    }
    if (col_no == -1)
    {
      mysql_free_result(res);
      report_or_die("Could not find column '%s' in the result of '%s'",
                    ds_col.str, ds_query.str);
      dynstr_free(&ds_query);
      dynstr_free(&ds_col);
      return;
    }
    DBUG_PRINT("info", ("Found column %d with name '%s'",
                        i, fields[i].name));
  }
  dynstr_free(&ds_col);

  {
    /* Get the value */
    MYSQL_ROW row;
    long rows= 0;
    const char* value= "No such row";

    while ((row= mysql_fetch_row(res)))
    {
      if (++rows == row_no)
      {

        DBUG_PRINT("info", ("At row %ld, column %d is '%s'",
                            row_no, col_no, row[col_no]));
        /* Found the row to get */
        if (row[col_no])
          value= row[col_no];
        else
          value= "NULL";

        break;
      }
    }
    eval_expr(var, value, 0, false, false);
  }
  dynstr_free(&ds_query);
  mysql_free_result(res);

  DBUG_VOID_RETURN;
}