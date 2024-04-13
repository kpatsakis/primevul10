try_get_value (const char *buf,
               const char *key,
               char **destination)
{
  const char *spos;
  const char *bpos;
  const char *endv;
  size_t klen;
  size_t vlen;

  if (NULL != *destination)
    return;
  bpos = buf;
  klen = strlen (key);
  while (NULL != (spos = strstr (bpos, key)))
  {
    if ( (spos[klen] != '=') ||
         ( (spos != buf) &&
           (spos[-1] != ' ') ) )
    {
      /* no match */
      bpos = spos + 1;
      continue;
    }
    if (spos[klen + 1] != '"')
      return;                   /* not quoted */
    if (NULL == (endv = strchr (&spos[klen + 2],
                                '\"')))
      return;                   /* no end-quote */
    vlen = endv - spos - klen - 1;
    *destination = malloc (vlen);
    if (NULL == *destination)
      return;                   /* out of memory */
    (*destination)[vlen - 1] = '\0';
    memcpy (*destination,
            &spos[klen + 2],
            vlen - 1);
    return;                     /* success */
  }
}