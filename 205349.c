static int match_expected_error(struct st_command *command,
                                unsigned int err_errno,
                                const char *err_sqlstate)
{
  uint i;

  for (i= 0 ; (uint) i < command->expected_errors.count ; i++)
  {
    if ((command->expected_errors.err[i].type == ERR_ERRNO) &&
        (command->expected_errors.err[i].code.errnum == err_errno))
      return i;

    if (command->expected_errors.err[i].type == ERR_SQLSTATE)
    {
      /*
        NULL is quite likely, but not in conjunction with a SQL-state expect!
      */
      if (unlikely(err_sqlstate == NULL))
        die("expecting a SQL-state (%s) from query '%s' which cannot "
            "produce one...",
            command->expected_errors.err[i].code.sqlstate, command->query);

      if (strncmp(command->expected_errors.err[i].code.sqlstate,
                  err_sqlstate, SQLSTATE_LENGTH) == 0)
        return i;
    }
  }
  return -1;
}