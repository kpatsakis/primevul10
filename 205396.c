static int do_read_query_result(struct st_connection *cn)
{
  DBUG_ASSERT(cn->has_thread);
  wait_query_thread_done(cn);
  if (cn->result)
    goto exit_func;

  signal_connection_thd(cn, EMB_READ_QUERY_RESULT);
  wait_query_thread_done(cn);

exit_func:
  return cn->result;
}