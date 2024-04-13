void set_reconnect(MYSQL* mysql, my_bool val)
{
  my_bool reconnect= val;
  DBUG_ENTER("set_reconnect");
  DBUG_PRINT("info", ("val: %d", (int) val));
#if MYSQL_VERSION_ID < 50000
  mysql->reconnect= reconnect;
#else
  mysql_options(mysql, MYSQL_OPT_RECONNECT, (char *)&reconnect);
#endif
  DBUG_VOID_RETURN;
}