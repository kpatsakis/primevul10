static void handle_no_active_connection(struct st_command *command, 
  struct st_connection *cn, DYNAMIC_STRING *ds)
{
  handle_error(command, 2006, "MySQL server has gone away", "000000", ds);
  cn->pending= FALSE;
  var_set_errno(2006);
}