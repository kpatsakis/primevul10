get_one_option(int optid, const struct my_option *opt, char *argument)
{
  switch(optid) {
  case '#':
#ifndef DBUG_OFF
    DBUG_PUSH(argument ? argument : "d:t:S:i:O,/tmp/mysqltest.trace");
    debug_check_flag= 1;
    debug_info_flag= 1;
#endif
    break;
  case 'r':
    record = 1;
    break;
  case 'x':
  {
    char buff[FN_REFLEN];
    if (!test_if_hard_path(argument))
    {
      strxmov(buff, opt_basedir, argument, NullS);
      argument= buff;
    }
    fn_format(buff, argument, "", "", MY_UNPACK_FILENAME);
    DBUG_ASSERT(cur_file == file_stack && cur_file->file == 0);
    if (!(cur_file->file=
          fopen(buff, "rb")))
      die("Could not open '%s' for reading, errno: %d", buff, errno);
    cur_file->file_name= my_strdup(buff, MYF(MY_FAE));
    cur_file->lineno= 1;
    break;
  }
  case 'm':
  {
    static char buff[FN_REFLEN];
    if (!test_if_hard_path(argument))
    {
      strxmov(buff, opt_basedir, argument, NullS);
      argument= buff;
    }
    fn_format(buff, argument, "", "", MY_UNPACK_FILENAME);
    timer_file= buff;
    unlink(timer_file);	     /* Ignore error, may not exist */
    break;
  }
  case 'p':
    if (argument == disabled_my_option)
      argument= (char*) "";			// Don't require password
    if (argument)
    {
      my_free(opt_pass);
      opt_pass= my_strdup(argument, MYF(MY_FAE));
      while (*argument) *argument++= 'x';		/* Destroy argument */
      tty_password= 0;
    }
    else
      tty_password= 1;
    break;
#include <sslopt-case.h>
  case 't':
    strnmov(TMPDIR, argument, sizeof(TMPDIR));
    break;
  case 'A':
    if (!embedded_server_arg_count)
    {
      embedded_server_arg_count=1;
      embedded_server_args[0]= (char*) "";
    }
    if (embedded_server_arg_count == MAX_EMBEDDED_SERVER_ARGS-1 ||
        !(embedded_server_args[embedded_server_arg_count++]=
          my_strdup(argument, MYF(MY_FAE))))
    {
      die("Can't use server argument");
    }
    break;
  case OPT_LOG_DIR:
    /* Check that the file exists */
    if (access(opt_logdir, F_OK) != 0)
      die("The specified log directory does not exist: '%s'", opt_logdir);
    break;
  case 'F':
    read_embedded_server_arguments(argument);
    break;
  case OPT_RESULT_FORMAT_VERSION:
    set_result_format_version(opt_result_format_version);
    break;
  case 'V':
    print_version();
    exit(0);
  case OPT_MYSQL_PROTOCOL:
#ifndef EMBEDDED_LIBRARY
    opt_protocol= find_type_or_exit(argument, &sql_protocol_typelib,
                                    opt->name);
#endif
    break;
  case '?':
    usage();
    exit(0);
  }
  return 0;
}