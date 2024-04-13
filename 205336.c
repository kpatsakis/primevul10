void close_connections()
{
  DBUG_ENTER("close_connections");
  for (--next_con; next_con >= connections; --next_con)
  {
#ifdef EMBEDDED_LIBRARY
    emb_close_connection(next_con);
#endif
    if (next_con->stmt)
      mysql_stmt_close(next_con->stmt);
    next_con->stmt= 0;
    mysql_close(next_con->mysql);
    next_con->mysql= 0;
    if (next_con->util_mysql)
      mysql_close(next_con->util_mysql);
    my_free(next_con->name);
  }
  my_free(connections);
  DBUG_VOID_RETURN;
}