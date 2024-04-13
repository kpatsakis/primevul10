mysql_get_server_version(MYSQL *mysql)
{
  ulong major= 0, minor= 0, version= 0;

  if (mysql->server_version)
  {
    const char *pos= mysql->server_version;
    char *end_pos;
    /* Skip possible prefix */
    while (*pos && !my_isdigit(&my_charset_latin1, *pos))
      pos++;
    major=   strtoul(pos, &end_pos, 10);	pos=end_pos+1;
    minor=   strtoul(pos, &end_pos, 10);	pos=end_pos+1;
    version= strtoul(pos, &end_pos, 10);
  }
  else
  {
    set_mysql_error(mysql, CR_COMMANDS_OUT_OF_SYNC, unknown_sqlstate);
  }

  return major*10000 + minor*100 + version;
}