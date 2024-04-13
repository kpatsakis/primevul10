void handle_no_error(struct st_command *command)
{
  DBUG_ENTER("handle_no_error");

  if (command->expected_errors.err[0].type == ERR_ERRNO &&
      command->expected_errors.err[0].code.errnum != 0)
  {
    /* Error code we wanted was != 0, i.e. not an expected success */
    report_or_die("query '%s' succeeded - should have failed with errno %d...",
                  command->query, command->expected_errors.err[0].code.errnum);
  }
  else if (command->expected_errors.err[0].type == ERR_SQLSTATE &&
           strcmp(command->expected_errors.err[0].code.sqlstate,"00000") != 0)
  {
    /* SQLSTATE we wanted was != "00000", i.e. not an expected success */
    report_or_die("query '%s' succeeded - should have failed with "
                  "sqlstate %s...",
                  command->query,
                  command->expected_errors.err[0].code.sqlstate);
  }
  DBUG_VOID_RETURN;
}