void do_remove_files_wildcard(struct st_command *command)
{
  int error= 0, sys_errno= 0;
  uint i;
  size_t directory_length;
  MY_DIR *dir_info;
  FILEINFO *file;
  char dir_separator[2];
  static DYNAMIC_STRING ds_directory;
  static DYNAMIC_STRING ds_wild;
  static DYNAMIC_STRING ds_file_to_remove;
  char dirname[FN_REFLEN];
  
  const struct command_arg rm_args[] = {
    { "directory", ARG_STRING, TRUE, &ds_directory,
      "Directory containing files to delete" },
    { "filename", ARG_STRING, FALSE, &ds_wild, "File pattern to delete" }
  };
  DBUG_ENTER("do_remove_files_wildcard");

  check_command_args(command, command->first_argument,
                     rm_args, sizeof(rm_args)/sizeof(struct command_arg),
                     ' ');
  fn_format(dirname, ds_directory.str, "", "", MY_UNPACK_FILENAME);

  DBUG_PRINT("info", ("listing directory: %s", dirname));
  /* Note that my_dir sorts the list if not given any flags */
  if (!(dir_info= my_dir(dirname, MYF(MY_DONT_SORT | MY_WANT_STAT | MY_WME))))
  {
    error= 1;
    sys_errno= my_errno;
    goto end;
  }
  init_dynamic_string(&ds_file_to_remove, dirname, 1024, 1024);
  dir_separator[0]= FN_LIBCHAR;
  dynstr_append_mem(&ds_file_to_remove, dir_separator, 1);
  directory_length= ds_file_to_remove.length;
  
  /* Set default wild chars for wild_compare, is changed in embedded mode */
  set_wild_chars(1);
  
  for (i= 0; i < (uint) dir_info->number_off_files; i++)
  {
    file= dir_info->dir_entry + i;
    /* Remove only regular files, i.e. no directories etc. */
    /* if (!MY_S_ISREG(file->mystat->st_mode)) */
    /* MY_S_ISREG does not work here on Windows, just skip directories */
    if (MY_S_ISDIR(file->mystat->st_mode))
      continue;
    if (ds_wild.length &&
        wild_compare(file->name, ds_wild.str, 0))
      continue;
    ds_file_to_remove.length= directory_length;
    dynstr_append(&ds_file_to_remove, file->name);
    DBUG_PRINT("info", ("removing file: %s", ds_file_to_remove.str));
    if ((error= (my_delete(ds_file_to_remove.str, MYF(MY_WME)) != 0)))
      sys_errno= my_errno;
    if (error)
      break;
  }
  set_wild_chars(0);
  my_dirend(dir_info);

end:
  handle_command_error(command, error, sys_errno);
  dynstr_free(&ds_directory);
  dynstr_free(&ds_wild);
  dynstr_free(&ds_file_to_remove);
  DBUG_VOID_RETURN;
}