initialize_request (const struct url *u, struct http_stat *hs, int *dt, struct url *proxy,
                    bool inhibit_keep_alive, bool *basic_auth_finished,
                    wgint *body_data_size, char **user, char **passwd, uerr_t *ret)
{
  bool head_only = !!(*dt & HEAD_ONLY);
  struct request *req;

  /* Prepare the request to send. */
  {
    char *meth_arg;
    const char *meth = "GET";
    if (head_only)
      meth = "HEAD";
    else if (opt.method)
      meth = opt.method;
    /* Use the full path, i.e. one that includes the leading slash and
       the query string.  E.g. if u->path is "foo/bar" and u->query is
       "param=value", full_path will be "/foo/bar?param=value".  */
    if (proxy
#ifdef HAVE_SSL
        /* When using SSL over proxy, CONNECT establishes a direct
           connection to the HTTPS server.  Therefore use the same
           argument as when talking to the server directly. */
        && u->scheme != SCHEME_HTTPS
#endif
        )
      meth_arg = xstrdup (u->url);
    else
      meth_arg = url_full_path (u);
    req = request_new (meth, meth_arg);
  }

  request_set_header (req, "Referer", (char *) hs->referer, rel_none);
  if (*dt & SEND_NOCACHE)
    {
      /* Cache-Control MUST be obeyed by all HTTP/1.1 caching mechanisms...  */
      request_set_header (req, "Cache-Control", "no-cache, must-revalidate", rel_none);

      /* ... but some HTTP/1.0 caches doesn't implement Cache-Control.  */
      request_set_header (req, "Pragma", "no-cache", rel_none);
    }
  if (*dt & IF_MODIFIED_SINCE)
    {
      char strtime[32];
      uerr_t err = time_to_rfc1123 (hs->orig_file_tstamp, strtime, countof (strtime));

      if (err != RETROK)
        {
          logputs (LOG_VERBOSE, _("Cannot convert timestamp to http format. "
                                  "Falling back to time 0 as last modification "
                                  "time.\n"));
          strcpy (strtime, "Thu, 01 Jan 1970 00:00:00 GMT");
        }
      request_set_header (req, "If-Modified-Since", xstrdup (strtime), rel_value);
    }
  if (hs->restval)
    request_set_header (req, "Range",
                        aprintf ("bytes=%s-",
                                 number_to_static_string (hs->restval)),
                        rel_value);
  SET_USER_AGENT (req);
  request_set_header (req, "Accept", "*/*", rel_none);
#ifdef HAVE_LIBZ
  if (opt.compression != compression_none)
    request_set_header (req, "Accept-Encoding", "gzip", rel_none);
  else
#endif
    request_set_header (req, "Accept-Encoding", "identity", rel_none);

  /* Find the username with priority */
  if (u->user)
    *user = u->user;
  else if (opt.user && (opt.use_askpass || opt.ask_passwd))
    *user = opt.user;
  else if (opt.http_user)
    *user = opt.http_user;
  else if (opt.user)
    *user = opt.user;
  else
    *user = NULL;

  /* Find the password with priority */
  if (u->passwd)
    *passwd = u->passwd;
  else if (opt.passwd && (opt.use_askpass || opt.ask_passwd))
    *passwd = opt.passwd;
  else if (opt.http_passwd)
    *passwd = opt.http_passwd;
  else if (opt.passwd)
    *passwd = opt.passwd;
  else
    *passwd = NULL;

  /* Check for ~/.netrc if none of the above match */
  if (opt.netrc && (!*user || !*passwd))
    search_netrc (u->host, (const char **) user, (const char **) passwd, 0);

  /* We only do "site-wide" authentication with "global" user/password
   * values unless --auth-no-challange has been requested; URL user/password
   * info overrides. */
  if (*user && *passwd && (!u->user || opt.auth_without_challenge))
    {
      /* If this is a host for which we've already received a Basic
       * challenge, we'll go ahead and send Basic authentication creds. */
      *basic_auth_finished = maybe_send_basic_creds (u->host, *user, *passwd, req);
    }

  /* Generate the Host header, HOST:PORT.  Take into account that:

     - Broken server-side software often doesn't recognize the PORT
       argument, so we must generate "Host: www.server.com" instead of
       "Host: www.server.com:80" (and likewise for https port).

     - IPv6 addresses contain ":", so "Host: 3ffe:8100:200:2::2:1234"
       becomes ambiguous and needs to be rewritten as "Host:
       [3ffe:8100:200:2::2]:1234".  */
  {
    /* Formats arranged for hfmt[add_port][add_squares].  */
    static const char *hfmt[][2] = {
      { "%s", "[%s]" }, { "%s:%d", "[%s]:%d" }
    };
    int add_port = u->port != scheme_default_port (u->scheme);
    int add_squares = strchr (u->host, ':') != NULL;
    request_set_header (req, "Host",
                        aprintf (hfmt[add_port][add_squares], u->host, u->port),
                        rel_value);
  }

  if (inhibit_keep_alive)
    request_set_header (req, "Connection", "Close", rel_none);
  else
    {
      request_set_header (req, "Connection", "Keep-Alive", rel_none);
      if (proxy)
        request_set_header (req, "Proxy-Connection", "Keep-Alive", rel_none);
    }

  if (opt.method)
    {

      if (opt.body_data || opt.body_file)
        {
          request_set_header (req, "Content-Type",
                              "application/x-www-form-urlencoded", rel_none);

          if (opt.body_data)
            *body_data_size = strlen (opt.body_data);
          else
            {
              *body_data_size = file_size (opt.body_file);
              if (*body_data_size == -1)
                {
                  logprintf (LOG_NOTQUIET, _("BODY data file %s missing: %s\n"),
                             quote (opt.body_file), strerror (errno));
                  request_free (&req);
                  *ret = FILEBADFILE;
                  return NULL;
                }
            }
          request_set_header (req, "Content-Length",
                              xstrdup (number_to_static_string (*body_data_size)),
                              rel_value);
        }
      else if (c_strcasecmp (opt.method, "post") == 0
               || c_strcasecmp (opt.method, "put") == 0
               || c_strcasecmp (opt.method, "patch") == 0)
        request_set_header (req, "Content-Length", "0", rel_none);
    }
  return req;
}