void do_chmod_file(struct st_command *command)
{
  long mode= 0;
  int err_code;
  static DYNAMIC_STRING ds_mode;
  static DYNAMIC_STRING ds_file;
  const struct command_arg chmod_file_args[] = {
    { "mode", ARG_STRING, TRUE, &ds_mode, "Mode of file(octal) ex. 0660"}, 
    { "filename", ARG_STRING, TRUE, &ds_file, "Filename of file to modify" }
  };
  DBUG_ENTER("do_chmod_file");

  check_command_args(command, command->first_argument,
                     chmod_file_args,
                     sizeof(chmod_file_args)/sizeof(struct command_arg),
                     ' ');

  /* Parse what mode to set */
  if (ds_mode.length != 4 ||
      str2int(ds_mode.str, 8, 0, INT_MAX, &mode) == NullS)
    die("You must write a 4 digit octal number for mode");

  DBUG_PRINT("info", ("chmod %o %s", (uint)mode, ds_file.str));
  err_code= chmod(ds_file.str, mode);
  if (err_code < 0)
    err_code= 1;
  handle_command_error(command, err_code, errno);
  dynstr_free(&ds_mode);
  dynstr_free(&ds_file);
  DBUG_VOID_RETURN;
}