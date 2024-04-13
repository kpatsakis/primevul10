pthread_handler_t connection_thread(void *arg)
{
  struct st_connection *cn= (struct st_connection*)arg;

  mysql_thread_init();
  while (cn->command != EMB_END_CONNECTION)
  {
    if (!cn->command)
    {
      pthread_mutex_lock(&cn->query_mutex);
      while (!cn->command)
        pthread_cond_wait(&cn->query_cond, &cn->query_mutex);
      pthread_mutex_unlock(&cn->query_mutex);
    }
    switch (cn->command)
    {
      case EMB_END_CONNECTION:
        goto end_thread;
      case EMB_SEND_QUERY:
        cn->result= mysql_send_query(cn->mysql,
                                     cn->cur_query, cn->cur_query_len);
        break;
      case EMB_READ_QUERY_RESULT:
        cn->result= mysql_read_query_result(cn->mysql);
        break;
      case EMB_PREPARE_STMT:
        cn->result= mysql_stmt_prepare(cn->stmt,
                                       cn->cur_query, cn->cur_query_len);
        break;
      case EMB_EXECUTE_STMT:
        cn->result= mysql_stmt_execute(cn->stmt);
        break;
      default:
        DBUG_ASSERT(0);
    }
    cn->command= 0;
    pthread_mutex_lock(&cn->result_mutex);
    cn->query_done= 1;
    pthread_cond_signal(&cn->result_cond);
    pthread_mutex_unlock(&cn->result_mutex);
  }

end_thread:
  cn->query_done= 1;
  mysql_thread_end();
  pthread_exit(0);
  return 0;
}