int query_get_string(MYSQL* mysql, const char* query,
                     int column, DYNAMIC_STRING* ds)
{
  MYSQL_RES *res= NULL;
  MYSQL_ROW row;

  if (mysql_query(mysql, query))
  {
    report_or_die("'%s' failed: %d %s", query,
                  mysql_errno(mysql), mysql_error(mysql));
    return 1;
  }
  if ((res= mysql_store_result(mysql)) == NULL)
  {
    report_or_die("Failed to store result: %d %s",
                  mysql_errno(mysql), mysql_error(mysql));
    return 1;
  }

  if ((row= mysql_fetch_row(res)) == NULL)
  {
    mysql_free_result(res);
    return 1;
  }
  init_dynamic_string(ds, (row[column] ? row[column] : "NULL"), ~0, 32);
  mysql_free_result(res);
  return 0;
}