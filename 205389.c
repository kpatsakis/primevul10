void do_copy_file(struct st_command *command)
{
  int error;
  static DYNAMIC_STRING ds_from_file;
  static DYNAMIC_STRING ds_to_file;
  const struct command_arg copy_file_args[] = {
    { "from_file", ARG_STRING, TRUE, &ds_from_file, "Filename to copy from" },
    { "to_file", ARG_STRING, TRUE, &ds_to_file, "Filename to copy to" }
  };
  DBUG_ENTER("do_copy_file");

  check_command_args(command, command->first_argument,
                     copy_file_args,
                     sizeof(copy_file_args)/sizeof(struct command_arg),
                     ' ');

  DBUG_PRINT("info", ("Copy %s to %s", ds_from_file.str, ds_to_file.str));
  /* MY_HOLD_ORIGINAL_MODES prevents attempts to chown the file */
  error= (my_copy(ds_from_file.str, ds_to_file.str,
                  MYF(MY_DONT_OVERWRITE_FILE | MY_WME | MY_HOLD_ORIGINAL_MODES)) != 0);
  handle_command_error(command, error, my_errno);
  dynstr_free(&ds_from_file);
  dynstr_free(&ds_to_file);
  DBUG_VOID_RETURN;
}