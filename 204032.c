maybe_send_basic_creds (const char *hostname, const char *user,
                        const char *passwd, struct request *req)
{
  bool do_challenge = false;

  if (opt.auth_without_challenge)
    {
      DEBUGP (("Auth-without-challenge set, sending Basic credentials.\n"));
      do_challenge = true;
    }
  else if (basic_authed_hosts
      && hash_table_contains (basic_authed_hosts, hostname))
    {
      DEBUGP (("Found %s in basic_authed_hosts.\n", quote (hostname)));
      do_challenge = true;
    }
  else
    {
      DEBUGP (("Host %s has not issued a general basic challenge.\n",
              quote (hostname)));
    }
  if (do_challenge)
    {
      request_set_header (req, "Authorization",
                          basic_authentication_encode (user, passwd),
                          rel_value);
    }
  return do_challenge;
}