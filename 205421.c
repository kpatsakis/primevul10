int parse_args(int argc, char **argv)
{
  if (load_defaults("my",load_default_groups,&argc,&argv))
    exit(1);

  default_argv= argv;

  if ((handle_options(&argc, &argv, my_long_options, get_one_option)))
    exit(1);

  if (argc > 1)
  {
    usage();
    exit(1);
  }
  if (argc == 1)
    opt_db= *argv;
  if (tty_password)
    opt_pass= get_tty_password(NullS);          /* purify tested */
  if (debug_info_flag)
    my_end_arg= MY_CHECK_ERROR | MY_GIVE_INFO;
  if (debug_check_flag)
    my_end_arg|= MY_CHECK_ERROR;

  if (global_subst != NULL)
  {
    char *comma= strstr(global_subst, ",");
    if (comma == NULL)
      die("wrong --global-subst, must be X,Y");
    memcpy(global_subst_from, global_subst, (comma-global_subst));
    global_subst_from[comma-global_subst]= 0;
    memcpy(global_subst_to, comma+1, strlen(comma));
  }

  if (!opt_suite_dir)
    opt_suite_dir= "./";
  suite_dir_len= strlen(opt_suite_dir);
  overlay_dir_len= opt_overlay_dir ? strlen(opt_overlay_dir) : 0;

  if (!record)
  {
    /* Check that the result file exists */
    if (result_file_name && access(result_file_name, F_OK) != 0)
      die("The specified result file '%s' does not exist",
          result_file_name);
  }

  return 0;
}