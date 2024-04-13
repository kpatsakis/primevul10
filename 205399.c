void do_rmdir(struct st_command *command)
{
  int error;
  static DYNAMIC_STRING ds_dirname;
  const struct command_arg rmdir_args[] = {
    { "dirname", ARG_STRING, TRUE, &ds_dirname, "Directory to remove" }
  };
  DBUG_ENTER("do_rmdir");

  check_command_args(command, command->first_argument,
                     rmdir_args, sizeof(rmdir_args)/sizeof(struct command_arg),
                     ' ');

  DBUG_PRINT("info", ("removing directory: %s", ds_dirname.str));
  error= rmdir(ds_dirname.str) != 0;
  handle_command_error(command, error, errno);
  dynstr_free(&ds_dirname);
  DBUG_VOID_RETURN;
}