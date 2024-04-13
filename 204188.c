init_control_ssl_connection (int csock, struct url *u, bool *using_control_security)
{
  bool using_security = false;

  /* If '--ftps-implicit' was passed, perform the SSL handshake directly,
   * and do not send an AUTH command.
   * Otherwise send an AUTH sequence before login,
   * and perform the SSL handshake if accepted by server.
   */
  if (!opt.ftps_implicit && !opt.server_response)
    logputs (LOG_VERBOSE, "==> AUTH TLS ... ");
  if (opt.ftps_implicit || ftp_auth (csock, SCHEME_FTPS) == FTPOK)
    {
      if (!ssl_connect_wget (csock, u->host, NULL))
        {
          fd_close (csock);
          return CONSSLERR;
        }
      else if (!ssl_check_certificate (csock, u->host))
        {
          fd_close (csock);
          return VERIFCERTERR;
        }

      if (!opt.ftps_implicit && !opt.server_response)
        logputs (LOG_VERBOSE, " done.\n");

      /* If implicit FTPS was requested, we act as "normal" FTP, but over SSL.
       * We're not using RFC 2228 commands.
       */
      using_security = true;
    }
  else
    {
      /* The server does not support 'AUTH TLS'.
       * Check if --ftps-fallback-to-ftp was passed. */
      if (opt.ftps_fallback_to_ftp)
        {
          logputs (LOG_NOTQUIET, "Server does not support AUTH TLS. Falling back to FTP.\n");
          using_security = false;
        }
      else
        {
          fd_close (csock);
          return FTPNOAUTH;
        }
    }

  *using_control_security = using_security;
  return NOCONERROR;
}