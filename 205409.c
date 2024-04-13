void var_query_set(VAR *var, const char *query, const char** query_end)
{
  char *end = (char*)((query_end && *query_end) ?
		      *query_end : query + strlen(query));
  MYSQL_RES *res;
  MYSQL_ROW row;
  MYSQL* mysql = cur_con->mysql;
  DYNAMIC_STRING ds_query;
  DBUG_ENTER("var_query_set");
  LINT_INIT(res);

  if (!mysql)
  {
    struct st_command command;
    memset(&command, 0, sizeof(command));
    command.query= (char*)query;
    command.first_word_len= (*query_end - query);
    command.first_argument= command.query + command.first_word_len;
    command.end= (char*)*query_end;
    command.abort_on_error= 1; /* avoid uninitialized variables */
    handle_no_active_connection(&command, cur_con, &ds_res);
    DBUG_VOID_RETURN;
  }

  /* Only white space or ) allowed past ending ` */
  while (end > query && *end != '`')
  {
    if (*end && (*end != ' ' && *end != '\t' && *end != '\n' && *end != ')'))
      die("Spurious text after `query` expression");
    --end;
  }

  if (query == end)
    die("Syntax error in query, missing '`'");
  ++query;

  /* Eval the query, thus replacing all environment variables */
  init_dynamic_string(&ds_query, 0, (end - query) + 32, 256);
  do_eval(&ds_query, query, end, FALSE);

  if (mysql_real_query(mysql, ds_query.str, ds_query.length)) 
  {
    handle_error(curr_command, mysql_errno(mysql), mysql_error(mysql),
                 mysql_sqlstate(mysql), &ds_res);
    /* If error was acceptable, return empty string */
    dynstr_free(&ds_query);
    eval_expr(var, "", 0);
    DBUG_VOID_RETURN;
  }
  
  if (!(res= mysql_store_result(mysql)))
  {
    report_or_die("Query '%s' didn't return a result set", ds_query.str);
    dynstr_free(&ds_query);
    eval_expr(var, "", 0);
    return;
  }
  dynstr_free(&ds_query);

  if ((row= mysql_fetch_row(res)) && row[0])
  {
    /*
      Concatenate all fields in the first row with tab in between
      and assign that string to the $variable
    */
    DYNAMIC_STRING result;
    uint i;
    ulong *lengths;

    init_dynamic_string(&result, "", 512, 512);
    lengths= mysql_fetch_lengths(res);
    for (i= 0; i < mysql_num_fields(res); i++)
    {
      if (row[i])
      {
        /* Add column to tab separated string */
	char *val= row[i];
	int len= lengths[i];
	
	if (glob_replace_regex)
	{
	  /* Regex replace */
	  if (!multi_reg_replace(glob_replace_regex, (char*)val))
	  {
	    val= glob_replace_regex->buf;
	    len= strlen(val);
	  }
	}
	
	if (glob_replace)
	  replace_strings_append(glob_replace, &result, val, len);
	else
	  dynstr_append_mem(&result, val, len);
      }
      dynstr_append_mem(&result, "\t", 1);
    }
    end= result.str + result.length-1;
    /* Evaluation should not recurse via backtick */
    eval_expr(var, result.str, (const char**) &end, false, false);
    dynstr_free(&result);
  }
  else
    eval_expr(var, "", 0);

  mysql_free_result(res);
  DBUG_VOID_RETURN;
}