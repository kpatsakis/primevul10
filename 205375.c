void do_wait_for_slave_to_stop(struct st_command *c __attribute__((unused)))
{
  static int SLAVE_POLL_INTERVAL= 300000;
  MYSQL* mysql = cur_con->mysql;
  for (;;)
  {
    MYSQL_RES *UNINIT_VAR(res);
    MYSQL_ROW row;
    int done;

    if (mysql_query(mysql,"show status like 'Slave_running'") ||
	!(res=mysql_store_result(mysql)))
      die("Query failed while probing slave for stop: %s",
	  mysql_error(mysql));
    if (!(row=mysql_fetch_row(res)) || !row[1])
    {
      mysql_free_result(res);
      die("Strange result from query while probing slave for stop");
    }
    done = !strcmp(row[1],"OFF");
    mysql_free_result(res);
    if (done)
      break;
    my_sleep(SLAVE_POLL_INTERVAL);
  }
  return;
}