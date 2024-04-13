void handle_command_error(struct st_command *command, uint error,
                          int sys_errno)
{
  DBUG_ENTER("handle_command_error");
  DBUG_PRINT("enter", ("error: %d", error));
  var_set_int("$sys_errno",sys_errno);
  var_set_int("$errno",error);
  if (error != 0)
  {
    int i;

    if (command->abort_on_error)
    {
      report_or_die("command \"%.*s\" failed with error: %u  my_errno: %d  "
                    "errno: %d",
          command->first_word_len, command->query, error, my_errno,
          sys_errno);
      DBUG_VOID_RETURN;
    }

    i= match_expected_error(command, error, NULL);

    if (i >= 0)
    {
      DBUG_PRINT("info", ("command \"%.*s\" failed with expected error: %u, errno: %d",
                          command->first_word_len, command->query, error,
                          sys_errno));
      revert_properties();
      DBUG_VOID_RETURN;
    }
    if (command->expected_errors.count > 0)
      report_or_die("command \"%.*s\" failed with wrong error: %u  "
                    "my_errno: %d  errno: %d",
                    command->first_word_len, command->query, error, my_errno,
                    sys_errno);
  }
  else if (command->expected_errors.err[0].type == ERR_ERRNO &&
           command->expected_errors.err[0].code.errnum != 0)
  {
    /* Error code we wanted was != 0, i.e. not an expected success */
    report_or_die("command \"%.*s\" succeeded - should have failed with "
                  "errno %d...",
        command->first_word_len, command->query,
        command->expected_errors.err[0].code.errnum);
  }
  revert_properties();
  DBUG_VOID_RETURN;
}