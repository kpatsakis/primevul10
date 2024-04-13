establish_connection (const struct url *u, const struct url **conn_ref,
                      struct http_stat *hs, struct url *proxy,
                      char **proxyauth,
                      struct request **req_ref, bool *using_ssl,
                      bool inhibit_keep_alive,
                      int *sock_ref)
{
  bool host_lookup_failed = false;
  int sock = *sock_ref;
  struct request *req = *req_ref;
  const struct url *conn = *conn_ref;
  struct response *resp;
  int write_error;
  int statcode;

  if (! inhibit_keep_alive)
    {
      /* Look for a persistent connection to target host, unless a
         proxy is used.  The exception is when SSL is in use, in which
         case the proxy is nothing but a passthrough to the target
         host, registered as a connection to the latter.  */
      const struct url *relevant = conn;
#ifdef HAVE_SSL
      if (u->scheme == SCHEME_HTTPS)
        relevant = u;
#endif

      if (persistent_available_p (relevant->host, relevant->port,
#ifdef HAVE_SSL
                                  relevant->scheme == SCHEME_HTTPS,
#else
                                  0,
#endif
                                  &host_lookup_failed))
        {
          int family = socket_family (pconn.socket, ENDPOINT_PEER);
          sock = pconn.socket;
          *using_ssl = pconn.ssl;
#if ENABLE_IPV6
          if (family == AF_INET6)
             logprintf (LOG_VERBOSE, _("Reusing existing connection to [%s]:%d.\n"),
                        quotearg_style (escape_quoting_style, pconn.host),
                         pconn.port);
          else
#endif
             logprintf (LOG_VERBOSE, _("Reusing existing connection to %s:%d.\n"),
                        quotearg_style (escape_quoting_style, pconn.host),
                        pconn.port);
          DEBUGP (("Reusing fd %d.\n", sock));
          if (pconn.authorized)
            /* If the connection is already authorized, the "Basic"
               authorization added by code above is unnecessary and
               only hurts us.  */
            request_remove_header (req, "Authorization");
        }
      else if (host_lookup_failed)
        {
          logprintf(LOG_NOTQUIET,
                    _("%s: unable to resolve host address %s\n"),
                    exec_name, quote (relevant->host));
          return HOSTERR;
        }
      else if (sock != -1)
        {
          sock = -1;
        }
    }

  if (sock < 0)
    {
      sock = connect_to_host (conn->host, conn->port);
      if (sock == E_HOST)
        return HOSTERR;
      else if (sock < 0)
        return (retryable_socket_connect_error (errno)
                ? CONERROR : CONIMPOSSIBLE);

#ifdef HAVE_SSL
      if (proxy && u->scheme == SCHEME_HTTPS)
        {
          char *head;
          char *message;
          /* When requesting SSL URLs through proxies, use the
             CONNECT method to request passthrough.  */
          struct request *connreq = request_new ("CONNECT",
                              aprintf ("%s:%d", u->host, u->port));
          SET_USER_AGENT (connreq);
          if (proxyauth)
            {
              request_set_header (connreq, "Proxy-Authorization",
                                  *proxyauth, rel_value);
              /* Now that PROXYAUTH is part of the CONNECT request,
                 zero it out so we don't send proxy authorization with
                 the regular request below.  */
              *proxyauth = NULL;
            }
          request_set_header (connreq, "Host",
                              aprintf ("%s:%d", u->host, u->port),
                              rel_value);

          write_error = request_send (connreq, sock, 0);
          request_free (&connreq);
          if (write_error < 0)
            {
              CLOSE_INVALIDATE (sock);
              return WRITEFAILED;
            }

          head = read_http_response_head (sock);
          if (!head)
            {
              logprintf (LOG_VERBOSE, _("Failed reading proxy response: %s\n"),
                         fd_errstr (sock));
              CLOSE_INVALIDATE (sock);
              return HERR;
            }
          message = NULL;
          if (!*head)
            {
              xfree (head);
              goto failed_tunnel;
            }
          DEBUGP (("proxy responded with: [%s]\n", head));

          resp = resp_new (head);
          statcode = resp_status (resp, &message);
          if (statcode < 0)
            {
              char *tms = datetime_str (time (NULL));
              logprintf (LOG_VERBOSE, "%d\n", statcode);
              logprintf (LOG_NOTQUIET, _("%s ERROR %d: %s.\n"), tms, statcode,
                         quotearg_style (escape_quoting_style,
                                         _("Malformed status line")));
              xfree (head);
              return HERR;
            }
          xfree (hs->message);
          hs->message = xstrdup (message);
          resp_free (&resp);
          xfree (head);
          if (statcode != 200)
            {
            failed_tunnel:
              logprintf (LOG_NOTQUIET, _("Proxy tunneling failed: %s"),
                         message ? quotearg_style (escape_quoting_style, message) : "?");
              xfree (message);
              return CONSSLERR;
            }
          xfree (message);

          /* SOCK is now *really* connected to u->host, so update CONN
             to reflect this.  That way register_persistent will
             register SOCK as being connected to u->host:u->port.  */
          conn = u;
        }

      if (conn->scheme == SCHEME_HTTPS)
        {
          if (!ssl_connect_wget (sock, u->host, NULL))
            {
              CLOSE_INVALIDATE (sock);
              return CONSSLERR;
            }
          else if (!ssl_check_certificate (sock, u->host))
            {
              CLOSE_INVALIDATE (sock);
              return VERIFCERTERR;
            }
          *using_ssl = true;
        }
#endif /* HAVE_SSL */
    }
  *conn_ref = conn;
  *req_ref = req;
  *sock_ref = sock;
  return RETROK;
}