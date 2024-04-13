static int do_stmt_execute(struct st_connection *cn)
{
  /* The cn->stmt is already set. */
  if (!cn->has_thread)
    return mysql_stmt_execute(cn->stmt);
  signal_connection_thd(cn, EMB_EXECUTE_STMT);
  wait_query_thread_done(cn);
  return cn->result;
}