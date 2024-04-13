mysql_get_socket(const MYSQL *mysql)
{
  if (mysql->net.vio)
    return mysql->net.vio->sd;
  return INVALID_SOCKET;
}