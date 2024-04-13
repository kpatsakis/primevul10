void do_move_file(struct st_command *command)
{
  int error;
  static DYNAMIC_STRING ds_from_file;
  static DYNAMIC_STRING ds_to_file;
  const struct command_arg move_file_args[] = {
    { "from_file", ARG_STRING, TRUE, &ds_from_file, "Filename to move from" },
    { "to_file", ARG_STRING, TRUE, &ds_to_file, "Filename to move to" }
  };
  DBUG_ENTER("do_move_file");

  check_command_args(command, command->first_argument,
                     move_file_args,
                     sizeof(move_file_args)/sizeof(struct command_arg),
                     ' ');

  DBUG_PRINT("info", ("Move %s to %s", ds_from_file.str, ds_to_file.str));
  error= (my_rename(ds_from_file.str, ds_to_file.str,
                    MYF(disable_warnings ? 0 : MY_WME)) != 0);
  handle_command_error(command, error, my_errno);
  dynstr_free(&ds_from_file);
  dynstr_free(&ds_to_file);
  DBUG_VOID_RETURN;
}