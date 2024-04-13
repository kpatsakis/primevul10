void do_delimiter(struct st_command* command)
{
  char* p= command->first_argument;
  DBUG_ENTER("do_delimiter");
  DBUG_PRINT("enter", ("first_argument: %s", command->first_argument));

  while (*p && my_isspace(charset_info, *p))
    p++;

  if (!(*p))
    die("Can't set empty delimiter");

  delimiter_length= strmake_buf(delimiter, p) - delimiter;

  DBUG_PRINT("exit", ("delimiter: %s", delimiter));
  command->last_argument= p + delimiter_length;
  DBUG_VOID_RETURN;
}