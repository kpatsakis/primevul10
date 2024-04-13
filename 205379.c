void check_result()
{
  const char *mess= 0;

  DBUG_ENTER("check_result");
  DBUG_ASSERT(result_file_name);
  DBUG_PRINT("enter", ("result_file_name: %s", result_file_name));

  switch (compare_files(log_file.file_name(), result_file_name)) {
  case RESULT_OK:
    if (!error_count)
      break; /* ok */
    mess= "Got errors while running test";
    /* Fallthrough */
  case RESULT_LENGTH_MISMATCH:
    if (!mess)
      mess= "Result length mismatch\n";
    /* Fallthrough */
  case RESULT_CONTENT_MISMATCH:
  {
    /*
      Result mismatched, dump results to .reject file
      and then show the diff
    */
    char reject_file[FN_REFLEN];
    size_t reject_length;

    if (!mess)
      mess= "Result content mismatch\n";

    dirname_part(reject_file, result_file_name, &reject_length);

    if (access(reject_file, W_OK) == 0)
    {
      /* Result file directory is writable, save reject file there */
      fn_format(reject_file, result_file_name, "",
                ".reject", MY_REPLACE_EXT);
    }
    else
    {
      /* Put reject file in opt_logdir */
      fn_format(reject_file, result_file_name, opt_logdir,
                ".reject", MY_REPLACE_DIR | MY_REPLACE_EXT);
    }

    if (my_copy(log_file.file_name(), reject_file, MYF(0)) != 0)
      die("Failed to copy '%s' to '%s', errno: %d",
          log_file.file_name(), reject_file, errno);

    show_diff(NULL, result_file_name, reject_file);
    die("%s", mess);
    break;
  }
  default: /* impossible */
    die("Unknown error code from dyn_string_cmp()");
  }

  DBUG_VOID_RETURN;
}