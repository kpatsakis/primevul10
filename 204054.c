create_authorization_line (const char *au, const char *user,
                           const char *passwd, const char *method,
                           const char *path, bool *finished, uerr_t *auth_err)
{
  /* We are called only with known schemes, so we can dispatch on the
     first letter. */
  switch (c_toupper (*au))
    {
    case 'B':                   /* Basic */
      *finished = true;
      return basic_authentication_encode (user, passwd);
#ifdef ENABLE_DIGEST
    case 'D':                   /* Digest */
      *finished = true;
      return digest_authentication_encode (au, user, passwd, method, path, auth_err);
#endif
#ifdef ENABLE_NTLM
    case 'N':                   /* NTLM */
      if (!ntlm_input (&pconn.ntlm, au))
        {
          *finished = true;
          return NULL;
        }
      return ntlm_output (&pconn.ntlm, user, passwd, finished);
#endif
    default:
      /* We shouldn't get here -- this function should be only called
         with values approved by known_authentication_scheme_p.  */
      abort ();
    }
}