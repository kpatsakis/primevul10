initialize_proxy_configuration (const struct url *u, struct request *req,
                                struct url *proxy, char **proxyauth)
{
  char *proxy_user, *proxy_passwd;
  /* For normal username and password, URL components override
     command-line/wgetrc parameters.  With proxy
     authentication, it's the reverse, because proxy URLs are
     normally the "permanent" ones, so command-line args
     should take precedence.  */
  if (opt.proxy_user && opt.proxy_passwd)
    {
      proxy_user = opt.proxy_user;
      proxy_passwd = opt.proxy_passwd;
    }
  else
    {
      proxy_user = proxy->user;
      proxy_passwd = proxy->passwd;
    }
  /* #### This does not appear right.  Can't the proxy request,
     say, `Digest' authentication?  */
  if (proxy_user && proxy_passwd)
    *proxyauth = basic_authentication_encode (proxy_user, proxy_passwd);

  /* Proxy authorization over SSL is handled below. */
#ifdef HAVE_SSL
  if (u->scheme != SCHEME_HTTPS)
#endif
    request_set_header (req, "Proxy-Authorization", *proxyauth, rel_value);
}