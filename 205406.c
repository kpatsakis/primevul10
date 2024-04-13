static int do_stmt_prepare(struct st_connection *cn, const char *q, int q_len)
{
  /* The cn->stmt is already set. */
  if (!cn->has_thread)
    return mysql_stmt_prepare(cn->stmt, q, q_len);
  cn->cur_query= q;
  cn->cur_query_len= q_len;
  signal_connection_thd(cn, EMB_PREPARE_STMT);
  wait_query_thread_done(cn);
  return cn->result;
}