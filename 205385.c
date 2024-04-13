int util_query(MYSQL* org_mysql, const char* query){

  MYSQL* mysql;
  DBUG_ENTER("util_query");

  if(!(mysql= cur_con->util_mysql))
  {
    DBUG_PRINT("info", ("Creating util_mysql"));
    if (!(mysql= mysql_init(mysql)))
      die("Failed in mysql_init()");

    if (opt_connect_timeout)
      mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT,
                    (void *) &opt_connect_timeout);

    /* enable local infile, in non-binary builds often disabled by default */
    mysql_options(mysql, MYSQL_OPT_LOCAL_INFILE, 0);
    mysql_options(mysql, MYSQL_OPT_NONBLOCK, 0);
    safe_connect(mysql, "util", org_mysql->host, org_mysql->user,
                 org_mysql->passwd, org_mysql->db, org_mysql->port,
                 org_mysql->unix_socket);

    cur_con->util_mysql= mysql;
  }

  int ret= mysql_query(mysql, query);
  DBUG_RETURN(ret);
}