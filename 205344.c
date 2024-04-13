void do_send_quit(struct st_command *command)
{
  char *p= command->first_argument, *name;
  struct st_connection *con;

  DBUG_ENTER("do_send_quit");
  DBUG_PRINT("enter",("name: '%s'",p));

  if (!*p)
    die("Missing connection name in send_quit");
  name= p;
  while (*p && !my_isspace(charset_info,*p))
    p++;

  if (*p)
    *p++= 0;
  command->last_argument= p;

  if (!(con= find_connection_by_name(name)))
    die("connection '%s' not found in connection pool", name);

  simple_command(con->mysql,COM_QUIT,0,0,1);

  DBUG_VOID_RETURN;
}