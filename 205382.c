int do_sleep(struct st_command *command, my_bool real_sleep)
{
  int error= 0;
  char *sleep_start, *sleep_end;
  double sleep_val;
  char *p;
  static DYNAMIC_STRING ds_sleep;
  const struct command_arg sleep_args[] = {
    { "sleep_delay", ARG_STRING, TRUE, &ds_sleep, "Number of seconds to sleep." }
  };
  check_command_args(command, command->first_argument, sleep_args,
                     sizeof(sleep_args)/sizeof(struct command_arg),
                     ' ');

  p= ds_sleep.str;
  sleep_end= ds_sleep.str + ds_sleep.length;
  while (my_isspace(charset_info, *p))
    p++;
  if (!*p)
    die("Missing argument to %.*s", command->first_word_len,
        command->query);
  sleep_start= p;
  /* Check that arg starts with a digit, not handled by my_strtod */
  if (!my_isdigit(charset_info, *sleep_start))
    die("Invalid argument to %.*s \"%s\"", command->first_word_len,
        command->query, sleep_start);
  sleep_val= my_strtod(sleep_start, &sleep_end, &error);
  check_eol_junk_line(sleep_end);
  if (error)
    die("Invalid argument to %.*s \"%s\"", command->first_word_len,
        command->query, command->first_argument);
  dynstr_free(&ds_sleep);

  /* Fixed sleep time selected by --sleep option */
  if (opt_sleep >= 0 && !real_sleep)
    sleep_val= opt_sleep;

  DBUG_PRINT("info", ("sleep_val: %f", sleep_val));
  if (sleep_val)
    my_sleep((ulong) (sleep_val * 1000000L));
  return 0;
}