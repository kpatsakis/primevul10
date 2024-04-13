digest_authentication_encode (const char *au, const char *user,
                              const char *passwd, const char *method,
                              const char *path, uerr_t *auth_err)
{
  static char *realm, *opaque, *nonce, *qop, *algorithm;
  static struct {
    const char *name;
    char **variable;
  } options[] = {
    { "realm", &realm },
    { "opaque", &opaque },
    { "nonce", &nonce },
    { "qop", &qop },
    { "algorithm", &algorithm }
  };
  char cnonce[16] = "";
  char *res = NULL;
  int res_len;
  size_t res_size;
  param_token name, value;


  realm = opaque = nonce = algorithm = qop = NULL;

  au += 6;                      /* skip over `Digest' */
  while (extract_param (&au, &name, &value, ',', NULL))
    {
      size_t i;
      size_t namelen = name.e - name.b;
      for (i = 0; i < countof (options); i++)
        if (namelen == strlen (options[i].name)
            && 0 == strncmp (name.b, options[i].name,
                             namelen))
          {
            *options[i].variable = strdupdelim (value.b, value.e);
            break;
          }
    }

  if (qop && strcmp (qop, "auth"))
    {
      logprintf (LOG_NOTQUIET, _("Unsupported quality of protection '%s'.\n"), qop);
      xfree (qop); /* force freeing mem and continue */
    }
  else if (algorithm && strcmp (algorithm,"MD5") && strcmp (algorithm,"MD5-sess"))
    {
      logprintf (LOG_NOTQUIET, _("Unsupported algorithm '%s'.\n"), algorithm);
      xfree (algorithm); /* force freeing mem and continue */
    }

  if (!realm || !nonce || !user || !passwd || !path || !method)
    {
      *auth_err = ATTRMISSING;
      goto cleanup;
   }

  /* Calculate the digest value.  */
  {
    struct md5_ctx ctx;
    unsigned char hash[MD5_DIGEST_SIZE];
    char a1buf[MD5_DIGEST_SIZE * 2 + 1], a2buf[MD5_DIGEST_SIZE * 2 + 1];
    char response_digest[MD5_DIGEST_SIZE * 2 + 1];

    /* A1BUF = H(user ":" realm ":" password) */
    md5_init_ctx (&ctx);
    md5_process_bytes ((unsigned char *)user, strlen (user), &ctx);
    md5_process_bytes ((unsigned char *)":", 1, &ctx);
    md5_process_bytes ((unsigned char *)realm, strlen (realm), &ctx);
    md5_process_bytes ((unsigned char *)":", 1, &ctx);
    md5_process_bytes ((unsigned char *)passwd, strlen (passwd), &ctx);
    md5_finish_ctx (&ctx, hash);

    dump_hash (a1buf, hash);

    if (algorithm && !strcmp (algorithm, "MD5-sess"))
      {
        /* A1BUF = H( H(user ":" realm ":" password) ":" nonce ":" cnonce ) */
        snprintf (cnonce, sizeof (cnonce), "%08x",
          (unsigned) random_number (INT_MAX));

        md5_init_ctx (&ctx);
        /* md5_process_bytes (hash, MD5_DIGEST_SIZE, &ctx); */
        md5_process_bytes (a1buf, MD5_DIGEST_SIZE * 2, &ctx);
        md5_process_bytes ((unsigned char *)":", 1, &ctx);
        md5_process_bytes ((unsigned char *)nonce, strlen (nonce), &ctx);
        md5_process_bytes ((unsigned char *)":", 1, &ctx);
        md5_process_bytes ((unsigned char *)cnonce, strlen (cnonce), &ctx);
        md5_finish_ctx (&ctx, hash);

        dump_hash (a1buf, hash);
      }

    /* A2BUF = H(method ":" path) */
    md5_init_ctx (&ctx);
    md5_process_bytes ((unsigned char *)method, strlen (method), &ctx);
    md5_process_bytes ((unsigned char *)":", 1, &ctx);
    md5_process_bytes ((unsigned char *)path, strlen (path), &ctx);
    md5_finish_ctx (&ctx, hash);
    dump_hash (a2buf, hash);

    if (qop && !strcmp (qop, "auth"))
      {
        /* RFC 2617 Digest Access Authentication */
        /* generate random hex string */
        if (!*cnonce)
          snprintf (cnonce, sizeof (cnonce), "%08x",
            (unsigned) random_number (INT_MAX));

        /* RESPONSE_DIGEST = H(A1BUF ":" nonce ":" noncecount ":" clientnonce ":" qop ": " A2BUF) */
        md5_init_ctx (&ctx);
        md5_process_bytes ((unsigned char *)a1buf, MD5_DIGEST_SIZE * 2, &ctx);
        md5_process_bytes ((unsigned char *)":", 1, &ctx);
        md5_process_bytes ((unsigned char *)nonce, strlen (nonce), &ctx);
        md5_process_bytes ((unsigned char *)":", 1, &ctx);
        md5_process_bytes ((unsigned char *)"00000001", 8, &ctx); /* TODO: keep track of server nonce values */
        md5_process_bytes ((unsigned char *)":", 1, &ctx);
        md5_process_bytes ((unsigned char *)cnonce, strlen (cnonce), &ctx);
        md5_process_bytes ((unsigned char *)":", 1, &ctx);
        md5_process_bytes ((unsigned char *)qop, strlen (qop), &ctx);
        md5_process_bytes ((unsigned char *)":", 1, &ctx);
        md5_process_bytes ((unsigned char *)a2buf, MD5_DIGEST_SIZE * 2, &ctx);
        md5_finish_ctx (&ctx, hash);
      }
    else
      {
        /* RFC 2069 Digest Access Authentication */
        /* RESPONSE_DIGEST = H(A1BUF ":" nonce ":" A2BUF) */
        md5_init_ctx (&ctx);
        md5_process_bytes ((unsigned char *)a1buf, MD5_DIGEST_SIZE * 2, &ctx);
        md5_process_bytes ((unsigned char *)":", 1, &ctx);
        md5_process_bytes ((unsigned char *)nonce, strlen (nonce), &ctx);
        md5_process_bytes ((unsigned char *)":", 1, &ctx);
        md5_process_bytes ((unsigned char *)a2buf, MD5_DIGEST_SIZE * 2, &ctx);
        md5_finish_ctx (&ctx, hash);
      }

    dump_hash (response_digest, hash);

    res_size = strlen (user)
             + strlen (realm)
             + strlen (nonce)
             + strlen (path)
             + 2 * MD5_DIGEST_SIZE /*strlen (response_digest)*/
             + (opaque ? strlen (opaque) : 0)
             + (algorithm ? strlen (algorithm) : 0)
             + (qop ? 128: 0)
             + strlen (cnonce)
             + 128;

    res = xmalloc (res_size);

    if (qop && !strcmp (qop, "auth"))
      {
        res_len = snprintf (res, res_size, "Digest "\
                "username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\""\
                ", qop=auth, nc=00000001, cnonce=\"%s\"",
                  user, realm, nonce, path, response_digest, cnonce);

      }
    else
      {
        res_len = snprintf (res, res_size, "Digest "\
                "username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"",
                  user, realm, nonce, path, response_digest);
      }

    if (opaque)
      {
        res_len += snprintf (res + res_len, res_size - res_len, ", opaque=\"%s\"", opaque);
      }

    if (algorithm)
      {
        snprintf (res + res_len, res_size - res_len, ", algorithm=\"%s\"", algorithm);
      }
  }

cleanup:
  xfree (realm);
  xfree (opaque);
  xfree (nonce);
  xfree (qop);
  xfree (algorithm);

  return res;
}