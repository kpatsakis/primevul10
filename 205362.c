void do_remove_file(struct st_command *command)
{
  int error;
  static DYNAMIC_STRING ds_filename;
  const struct command_arg rm_args[] = {
    { "filename", ARG_STRING, TRUE, &ds_filename, "File to delete" }
  };
  DBUG_ENTER("do_remove_file");

  check_command_args(command, command->first_argument,
                     rm_args, sizeof(rm_args)/sizeof(struct command_arg),
                     ' ');

  DBUG_PRINT("info", ("removing file: %s", ds_filename.str));
  error= my_delete(ds_filename.str, MYF(disable_warnings ? 0 : MY_WME)) != 0;
  handle_command_error(command, error, my_errno);
  dynstr_free(&ds_filename);
  DBUG_VOID_RETURN;
}