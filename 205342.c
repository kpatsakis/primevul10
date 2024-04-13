void set_current_connection(struct st_connection *con)
{
  cur_con= con;
  /* Update $mysql_get_server_version to that of current connection */
  var_set_int("$mysql_get_server_version",
              mysql_get_server_version(con->mysql));
  /* Update $CURRENT_CONNECTION to the name of the current connection */
  var_set_string("$CURRENT_CONNECTION", con->name);
}