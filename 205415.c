void do_diff_files(struct st_command *command)
{
  int error= 0;
  static DYNAMIC_STRING ds_filename;
  static DYNAMIC_STRING ds_filename2;
  const struct command_arg diff_file_args[] = {
    { "file1", ARG_STRING, TRUE, &ds_filename, "First file to diff" },
    { "file2", ARG_STRING, TRUE, &ds_filename2, "Second file to diff" }
  };
  DBUG_ENTER("do_diff_files");

  check_command_args(command,
                     command->first_argument,
                     diff_file_args,
                     sizeof(diff_file_args)/sizeof(struct command_arg),
                     ' ');

  if (access(ds_filename.str, F_OK) != 0)
    die("command \"diff_files\" failed, file '%s' does not exist",
        ds_filename.str);

  if (access(ds_filename2.str, F_OK) != 0)
    die("command \"diff_files\" failed, file '%s' does not exist",
        ds_filename2.str);

  if ((error= compare_files(ds_filename.str, ds_filename2.str)) &&
      match_expected_error(command, error, NULL) < 0)
  {
    /*
      Compare of the two files failed, append them to output
      so the failure can be analyzed, but only if it was not
      expected to fail.
    */
    show_diff(&ds_res, ds_filename.str, ds_filename2.str);
    log_file.write(&ds_res);
    log_file.flush();
    dynstr_set(&ds_res, 0);
  }

  dynstr_free(&ds_filename);
  dynstr_free(&ds_filename2);
  handle_command_error(command, error, -1);
  DBUG_VOID_RETURN;
}