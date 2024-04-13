void do_cat_file(struct st_command *command)
{
  int error;
  static DYNAMIC_STRING ds_filename;
  const struct command_arg cat_file_args[] = {
    { "filename", ARG_STRING, TRUE, &ds_filename, "File to read from" }
  };
  DBUG_ENTER("do_cat_file");

  check_command_args(command,
                     command->first_argument,
                     cat_file_args,
                     sizeof(cat_file_args)/sizeof(struct command_arg),
                     ' ');

  DBUG_PRINT("info", ("Reading from, file: %s", ds_filename.str));

  error= cat_file(&ds_res, ds_filename.str);
  handle_command_error(command, error, my_errno);
  dynstr_free(&ds_filename);
  DBUG_VOID_RETURN;
}