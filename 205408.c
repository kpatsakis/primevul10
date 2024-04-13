void handle_error(struct st_command *command,
                  unsigned int err_errno, const char *err_error,
                  const char *err_sqlstate, DYNAMIC_STRING *ds)
{
  int i;

  DBUG_ENTER("handle_error");

  command->used_replace= 1;
  if (command->require_file)
  {
    /*
      The query after a "--require" failed. This is fine as long the server
      returned a valid reponse. Don't allow 2013 or 2006 to trigger an
      abort_not_supported_test
    */
    if (err_errno == CR_SERVER_LOST ||
        err_errno == CR_SERVER_GONE_ERROR)
      die("require query '%s' failed: %d: %s", command->query,
          err_errno, err_error);

    /* Abort the run of this test, pass the failed query as reason */
    abort_not_supported_test("Query '%s' failed, required functionality " \
                             "not supported", command->query);
  }

  if (command->abort_on_error)
  {
    report_or_die("query '%s' failed: %d: %s", command->query, err_errno,
                  err_error);
    DBUG_VOID_RETURN;
  }

  DBUG_PRINT("info", ("expected_errors.count: %d",
                      command->expected_errors.count));

  i= match_expected_error(command, err_errno, err_sqlstate);

  if (i >= 0)
  {
    if (!disable_result_log)
    {
      if (command->expected_errors.count == 1)
      {
        /* Only log error if there is one possible error */
        dynstr_append_mem(ds, "ERROR ", 6);
        replace_dynstr_append(ds, err_sqlstate);
        dynstr_append_mem(ds, ": ", 2);
        replace_dynstr_append(ds, err_error);
        dynstr_append_mem(ds,"\n",1);
      }
      /* Don't log error if we may not get an error */
      else if (command->expected_errors.err[0].type == ERR_SQLSTATE ||
               (command->expected_errors.err[0].type == ERR_ERRNO &&
                command->expected_errors.err[0].code.errnum != 0))
        dynstr_append(ds,"Got one of the listed errors\n");
    }
    /* OK */
    revert_properties();
    DBUG_VOID_RETURN;
  }

  DBUG_PRINT("info",("i: %d  expected_errors: %d", i,
                     command->expected_errors.count));

  if (!disable_result_log)
  {
    dynstr_append_mem(ds, "ERROR ",6);
    replace_dynstr_append(ds, err_sqlstate);
    dynstr_append_mem(ds, ": ", 2);
    replace_dynstr_append(ds, err_error);
    dynstr_append_mem(ds, "\n", 1);
  }

  if (command->expected_errors.count > 0)
  {
    if (command->expected_errors.err[0].type == ERR_ERRNO)
      report_or_die("query '%s' failed with wrong errno %d: '%s', instead of "
                    "%d...",
                    command->query, err_errno, err_error,
                    command->expected_errors.err[0].code.errnum);
    else
      report_or_die("query '%s' failed with wrong sqlstate %s: '%s', "
                    "instead of %s...",
                    command->query, err_sqlstate, err_error,
                    command->expected_errors.err[0].code.sqlstate);
  }

  revert_properties();
  DBUG_VOID_RETURN;
}