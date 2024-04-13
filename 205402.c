static void do_list_files_write_file_command(struct st_command *command,
                                             my_bool append)
{
  int error;
  static DYNAMIC_STRING ds_content;
  static DYNAMIC_STRING ds_filename;
  static DYNAMIC_STRING ds_dirname;
  static DYNAMIC_STRING ds_wild;
  const struct command_arg list_files_args[] = {
    {"filename", ARG_STRING, TRUE, &ds_filename, "Filename for write"},
    {"dirname", ARG_STRING, TRUE, &ds_dirname, "Directory to list"},
    {"file", ARG_STRING, FALSE, &ds_wild, "Filename (incl. wildcard)"}
  };
  DBUG_ENTER("do_list_files_write_file");
  command->used_replace= 1;

  check_command_args(command, command->first_argument,
                     list_files_args,
                     sizeof(list_files_args)/sizeof(struct command_arg), ' ');

  init_dynamic_string(&ds_content, "", 1024, 1024);
  error= get_list_files(&ds_content, &ds_dirname, &ds_wild);
  handle_command_error(command, error, my_errno);
  str_to_file2(ds_filename.str, ds_content.str, ds_content.length, append);
  dynstr_free(&ds_content);
  dynstr_free(&ds_filename);
  dynstr_free(&ds_dirname);
  dynstr_free(&ds_wild);
  DBUG_VOID_RETURN;
}