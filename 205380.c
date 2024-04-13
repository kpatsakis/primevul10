static void do_list_files(struct st_command *command)
{
  int error;
  static DYNAMIC_STRING ds_dirname;
  static DYNAMIC_STRING ds_wild;
  const struct command_arg list_files_args[] = {
    {"dirname", ARG_STRING, TRUE, &ds_dirname, "Directory to list"},
    {"file", ARG_STRING, FALSE, &ds_wild, "Filename (incl. wildcard)"}
  };
  DBUG_ENTER("do_list_files");
  command->used_replace= 1;

  check_command_args(command, command->first_argument,
                     list_files_args,
                     sizeof(list_files_args)/sizeof(struct command_arg), ' ');

  error= get_list_files(&ds_res, &ds_dirname, &ds_wild);
  handle_command_error(command, error, my_errno);
  dynstr_free(&ds_dirname);
  dynstr_free(&ds_wild);
  DBUG_VOID_RETURN;
}