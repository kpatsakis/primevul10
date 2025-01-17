static void test_bind_result()
{
  MYSQL_STMT *stmt;
  int        rc;
  int        nData;
  ulong      length1;
  char       szData[100];
  MYSQL_BIND my_bind[2];
  my_bool    is_null[2];

  myheader("test_bind_result");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_bind_result");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_bind_result(col1 int , col2 varchar(50))");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_bind_result VALUES(10, 'venu')");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_bind_result VALUES(20, 'MySQL')");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_bind_result(col2) VALUES('monty')");
  myquery(rc);

  rc= mysql_commit(mysql);
  myquery(rc);

  /* fetch */

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *) &nData;      /* integer data */
  my_bind[0].is_null= &is_null[0];

  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= szData;                /* string data */
  my_bind[1].buffer_length= sizeof(szData);
  my_bind[1].length= &length1;
  my_bind[1].is_null= &is_null[1];

  stmt= mysql_simple_prepare(mysql, "SELECT * FROM test_bind_result");
  check_stmt(stmt);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n row 1: %d, %s(%lu)", nData, szData, length1);
  DIE_UNLESS(nData == 10);
  DIE_UNLESS(strcmp(szData, "venu") == 0);
  DIE_UNLESS(length1 == 4);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n row 2: %d, %s(%lu)", nData, szData, length1);
  DIE_UNLESS(nData == 20);
  DIE_UNLESS(strcmp(szData, "MySQL") == 0);
  DIE_UNLESS(length1 == 5);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent && is_null[0])
    fprintf(stdout, "\n row 3: NULL, %s(%lu)", szData, length1);
  DIE_UNLESS(is_null[0]);
  DIE_UNLESS(strcmp(szData, "monty") == 0);
  DIE_UNLESS(length1 == 5);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);
}