void do_get_replace_column(struct st_command *command)
{
  char *from= command->first_argument;
  char *buff, *start;
  DBUG_ENTER("get_replace_columns");

  free_replace_column();
  if (!*from)
    die("Missing argument in %s", command->query);

  /* Allocate a buffer for results */
  start= buff= (char*)my_malloc(strlen(from)+1,MYF(MY_WME | MY_FAE));
  while (*from)
  {
    char *to;
    uint column_number;
    to= get_string(&buff, &from, command);
    if (!(column_number= atoi(to)) || column_number > MAX_COLUMNS)
      die("Wrong column number to replace_column in '%s'", command->query);
    if (!*from)
      die("Wrong number of arguments to replace_column in '%s'",
          command->query);
    to= get_string(&buff, &from, command);
    my_free(replace_column[column_number-1]);
    replace_column[column_number-1]= my_strdup(to, MYF(MY_WME | MY_FAE));
    set_if_bigger(max_replace_column, column_number);
  }
  my_free(start);
  command->last_argument= command->end;

  DBUG_VOID_RETURN;
}