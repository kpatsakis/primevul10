static void dump_backtrace(void)
{
  struct st_connection *conn= cur_con;

  fprintf(stderr, "read_command_buf (%p): ", read_command_buf);
  my_safe_print_str(read_command_buf, sizeof(read_command_buf));
  fputc('\n', stderr);

  if (conn)
  {
    fprintf(stderr, "conn->name (%p): ", conn->name);
    my_safe_print_str(conn->name, conn->name_len);
    fputc('\n', stderr);
#ifdef EMBEDDED_LIBRARY
    fprintf(stderr, "conn->cur_query (%p): ", conn->cur_query);
    my_safe_print_str(conn->cur_query, conn->cur_query_len);
    fputc('\n', stderr);
#endif
  }
  fputs("Attempting backtrace...\n", stderr);
  my_print_stacktrace(NULL, my_thread_stack_size);
}