void do_sync_with_master2(struct st_command *command, long offset)
{
  MYSQL_RES *res;
  MYSQL_ROW row;
  MYSQL *mysql= cur_con->mysql;
  char query_buf[FN_REFLEN+128];
  int timeout= 300; /* seconds */

  if (!master_pos.file[0])
    die("Calling 'sync_with_master' without calling 'save_master_pos'");

  sprintf(query_buf, "select master_pos_wait('%s', %ld, %d)",
          master_pos.file, master_pos.pos + offset, timeout);

  if (mysql_query(mysql, query_buf))
    die("failed in '%s': %d: %s", query_buf, mysql_errno(mysql),
        mysql_error(mysql));

  if (!(res= mysql_store_result(mysql)))
    die("mysql_store_result() returned NULL for '%s'", query_buf);
  if (!(row= mysql_fetch_row(res)))
  {
    mysql_free_result(res);
    die("empty result in %s", query_buf);
  }

  int result= -99;
  const char* result_str= row[0];
  if (result_str)
    result= atoi(result_str);

  mysql_free_result(res);

  if (!result_str || result < 0)
  {
    /* master_pos_wait returned NULL or < 0 */
    show_query(mysql, "SHOW MASTER STATUS");
    show_query(mysql, "SHOW SLAVE STATUS");
    show_query(mysql, "SHOW PROCESSLIST");
    fprintf(stderr, "analyze: sync_with_master\n");

    if (!result_str)
    {
      /*
        master_pos_wait returned NULL. This indicates that
        slave SQL thread is not started, the slave's master
        information is not initialized, the arguments are
        incorrect, or an error has occured
      */
      die("%.*s failed: '%s' returned NULL "          \
          "indicating slave SQL thread failure",
          command->first_word_len, command->query, query_buf);

    }

    if (result == -1)
      die("%.*s failed: '%s' returned -1 "            \
          "indicating timeout after %d seconds",
          command->first_word_len, command->query, query_buf, timeout);
    else
      die("%.*s failed: '%s' returned unknown result :%d",
          command->first_word_len, command->query, query_buf, result);
  }

  return;
}