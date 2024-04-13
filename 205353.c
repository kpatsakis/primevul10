void do_file_exist(struct st_command *command)
{
  int error;
  static DYNAMIC_STRING ds_filename;
  const struct command_arg file_exist_args[] = {
    { "filename", ARG_STRING, TRUE, &ds_filename, "File to check if it exist" }
  };
  DBUG_ENTER("do_file_exist");

  check_command_args(command, command->first_argument,
                     file_exist_args,
                     sizeof(file_exist_args)/sizeof(struct command_arg),
                     ' ');

  DBUG_PRINT("info", ("Checking for existence of file: %s", ds_filename.str));
  error= (access(ds_filename.str, F_OK) != 0);
  handle_command_error(command, error, errno);
  dynstr_free(&ds_filename);
  DBUG_VOID_RETURN;
}