void really_die(const char *msg)
{
  static int dying= 0;
  fflush(stdout);
  fprintf(stderr, "%s", msg);
  fflush(stderr);

  /*
    Protect against dying twice
    first time 'die' is called, try to write log files
    second time, just exit
  */
  if (dying)
    cleanup_and_exit(1);
  dying= 1;

  log_file.show_tail(opt_tail_lines);

  /*
    Help debugging by displaying any warnings that might have
    been produced prior to the error
  */
  if (cur_con && !cur_con->pending)
    show_warnings_before_error(cur_con->mysql);

  cleanup_and_exit(1);
}