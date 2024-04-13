test_cmd_spec_restrict_file_names(void)
{
  unsigned i;
  static const struct {
    const char *val;
    int expected_restrict_files_os;
    int expected_restrict_files_ctrl;
    int expected_restrict_files_case;
    bool result;
  } test_array[] = {
    { "windows", restrict_windows, true, restrict_no_case_restriction, true },
    { "windows,", restrict_windows, true, restrict_no_case_restriction, true },
    { "windows,lowercase", restrict_windows, true, restrict_lowercase, true },
    { "unix,nocontrol,lowercase,", restrict_unix, false, restrict_lowercase, true },
  };

  for (i = 0; i < countof(test_array); ++i)
    {
      bool res;

      defaults();
      res = cmd_spec_restrict_file_names ("dummy", test_array[i].val, NULL);

      /*
      fprintf (stderr, "test_cmd_spec_restrict_file_names: TEST %d\n", i); fflush (stderr);
      fprintf (stderr, "opt.restrict_files_os: %d\n",   opt.restrict_files_os); fflush (stderr);
      fprintf (stderr, "opt.restrict_files_ctrl: %d\n", opt.restrict_files_ctrl); fflush (stderr);
      fprintf (stderr, "opt.restrict_files_case: %d\n", opt.restrict_files_case); fflush (stderr);
      */
      mu_assert ("test_cmd_spec_restrict_file_names: wrong result",
                 res == test_array[i].result
                 && opt.restrict_files_os   == test_array[i].expected_restrict_files_os
                 && opt.restrict_files_ctrl == test_array[i].expected_restrict_files_ctrl
                 && opt.restrict_files_case == test_array[i].expected_restrict_files_case);
    }

  return NULL;
}