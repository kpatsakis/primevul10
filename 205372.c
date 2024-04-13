void do_system(struct st_command *command)
{
  DYNAMIC_STRING ds_cmd;
  DBUG_ENTER("do_system");

  if (strlen(command->first_argument) == 0)
  {
    report_or_die("Missing arguments to system, nothing to do!");
    return;
  }

  init_dynamic_string(&ds_cmd, 0, command->query_len + 64, 256);

  /* Eval the system command, thus replacing all environment variables */
  do_eval(&ds_cmd, command->first_argument, command->end, !is_windows);

#ifdef __WIN__
#ifndef USE_CYGWIN
   /* Replace /dev/null with NUL */
   while(replace(&ds_cmd, "/dev/null", 9, "NUL", 3) == 0)
     ;
#endif
#endif


  DBUG_PRINT("info", ("running system command '%s' as '%s'",
                      command->first_argument, ds_cmd.str));
  if (my_system(&ds_cmd))
  {
    if (command->abort_on_error)
      report_or_die("system command '%s' failed", command->first_argument);
    else
    {
      /* If ! abort_on_error, log message and continue */
      dynstr_append(&ds_res, "system command '");
      replace_dynstr_append(&ds_res, command->first_argument);
      dynstr_append(&ds_res, "' failed\n");
    }
  }

  command->last_argument= command->end;
  dynstr_free(&ds_cmd);
  DBUG_VOID_RETURN;
}