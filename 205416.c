void do_change_user(struct st_command *command)
{
  MYSQL *mysql = cur_con->mysql;
  /* static keyword to make the NetWare compiler happy. */
  static DYNAMIC_STRING ds_user, ds_passwd, ds_db;
  const struct command_arg change_user_args[] = {
    { "user", ARG_STRING, FALSE, &ds_user, "User to connect as" },
    { "password", ARG_STRING, FALSE, &ds_passwd, "Password used when connecting" },
    { "database", ARG_STRING, FALSE, &ds_db, "Database to select after connect" },
  };

  DBUG_ENTER("do_change_user");

  check_command_args(command, command->first_argument,
                     change_user_args,
                     sizeof(change_user_args)/sizeof(struct command_arg),
                     ',');

  if (cur_con->stmt)
  {
    mysql_stmt_close(cur_con->stmt);
    cur_con->stmt= NULL;
  }

  if (!ds_user.length)
  {
    dynstr_set(&ds_user, mysql->user);

    if (!ds_passwd.length)
      dynstr_set(&ds_passwd, mysql->passwd);

    if (!ds_db.length)
      dynstr_set(&ds_db, mysql->db);
  }

  DBUG_PRINT("info",("connection: '%s' user: '%s' password: '%s' database: '%s'",
                      cur_con->name, ds_user.str, ds_passwd.str, ds_db.str));

  if (mysql_change_user(mysql, ds_user.str, ds_passwd.str, ds_db.str))
    handle_error(command, mysql_errno(mysql), mysql_error(mysql),
		 mysql_sqlstate(mysql), &ds_res);
  else
    handle_no_error(command);

  dynstr_free(&ds_user);
  dynstr_free(&ds_passwd);
  dynstr_free(&ds_db);

  DBUG_VOID_RETURN;
}