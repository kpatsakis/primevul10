static int do_send_query(struct st_connection *cn, const char *q, int q_len)
{
  if (!cn->has_thread)
    return mysql_send_query(cn->mysql, q, q_len);
  cn->cur_query= q;
  cn->cur_query_len= q_len;
  signal_connection_thd(cn, EMB_SEND_QUERY);
  return 0;
}