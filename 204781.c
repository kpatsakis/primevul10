cli_safe_read(MYSQL *mysql)
{
  NET *net= &mysql->net;
  ulong len=0;

restart:
  if (net->vio != 0)
    len=my_net_read(net);

  if (len == packet_error || len == 0)
  {
    DBUG_PRINT("error",("Wrong connection or packet. fd: %s  len: %lu",
			vio_description(net->vio),len));
#ifdef MYSQL_SERVER
    if (net->vio && vio_was_interrupted(net->vio))
      return (packet_error);
#endif /*MYSQL_SERVER*/
    end_server(mysql);
    set_mysql_error(mysql, net->last_errno == ER_NET_PACKET_TOO_LARGE ?
                    CR_NET_PACKET_TOO_LARGE: CR_SERVER_LOST, unknown_sqlstate);
    return (packet_error);
  }
  if (net->read_pos[0] == 255)
  {
    if (len > 3)
    {
      uchar *pos= net->read_pos+1;
      uint last_errno=uint2korr(pos);

      if (last_errno == 65535 &&
          (mysql->server_capabilities & CLIENT_PROGRESS))
      {
        if (cli_report_progress(mysql, pos+2, (uint) (len-3)))
        {
          /* Wrong packet */
          set_mysql_error(mysql,  CR_MALFORMED_PACKET, unknown_sqlstate);
          return (packet_error);
        }
        goto restart;
      }
      net->last_errno= last_errno;
      
      pos+=2;
      len-=2;
      if (protocol_41(mysql) && (char) pos[0] == '#')
      {
	strmake_buf(net->sqlstate, (char*) pos+1);
	pos+= SQLSTATE_LENGTH+1;
      }
      else
      {
        /*
          The SQL state hasn't been received -- it should be reset to HY000
          (unknown error sql state).
        */

        strmov(net->sqlstate, unknown_sqlstate);
      }

      (void) strmake(net->last_error,(char*) pos,
		     min((uint) len,(uint) sizeof(net->last_error)-1));
    }
    else
      set_mysql_error(mysql, CR_UNKNOWN_ERROR, unknown_sqlstate);
    /*
      Cover a protocol design error: error packet does not
      contain the server status. Therefore, the client has no way
      to find out whether there are more result sets of
      a multiple-result-set statement pending. Luckily, in 5.0 an
      error always aborts execution of a statement, wherever it is
      a multi-statement or a stored procedure, so it should be
      safe to unconditionally turn off the flag here.
    */
    mysql->server_status&= ~SERVER_MORE_RESULTS_EXISTS;

    DBUG_PRINT("error",("Got error: %d/%s (%s)",
                        net->last_errno,
                        net->sqlstate,
                        net->last_error));
    return(packet_error);
  }
  return len;
}