idn_encode (const struct iri *i, const char *host)
{
  int ret;
  char *ascii_encoded;
  char *utf8_encoded = NULL;
  const char *src;
#if IDN2_VERSION_NUMBER < 0x00140000
  uint8_t *lower;
  size_t len = 0;
#endif

  /* Encode to UTF-8 if not done */
  if (!i->utf8_encode)
    {
      if (!remote_to_utf8 (i, host, &utf8_encoded))
          return NULL;  /* Nothing to encode or an error occurred */
      src = utf8_encoded;
    }
  else
    src = host;

#if IDN2_VERSION_NUMBER >= 0x00140000
  /* IDN2_TRANSITIONAL implies input NFC encoding */
  ret = idn2_lookup_u8 ((uint8_t *) src, (uint8_t **) &ascii_encoded, IDN2_NONTRANSITIONAL);
  if (ret != IDN2_OK)
    /* fall back to TR46 Transitional mode, max IDNA2003 compatibility */
    ret = idn2_lookup_u8 ((uint8_t *) src, (uint8_t **) &ascii_encoded, IDN2_TRANSITIONAL);

  if (ret != IDN2_OK)
    logprintf (LOG_VERBOSE, _("idn_encode failed (%d): %s\n"), ret,
               quote (idn2_strerror (ret)));
#else
  /* we need a conversion to lowercase */
  lower = u8_tolower ((uint8_t *) src, u8_strlen ((uint8_t *) src) + 1, 0, UNINORM_NFKC, NULL, &len);
  if (!lower)
    {
      logprintf (LOG_VERBOSE, _("Failed to convert to lower: %d: %s\n"),
                 errno, quote (src));
      xfree (utf8_encoded);
      return NULL;
    }

  if ((ret = idn2_lookup_u8 (lower, (uint8_t **) &ascii_encoded, IDN2_NFC_INPUT)) != IDN2_OK)
    {
      logprintf (LOG_VERBOSE, _("idn_encode failed (%d): %s\n"), ret,
                 quote (idn2_strerror (ret)));
    }

  xfree (lower);
#endif

  xfree (utf8_encoded);

  if (ret == IDN2_OK && ascii_encoded)
    {
      char *tmp = xstrdup (ascii_encoded);
      idn2_free (ascii_encoded);
      ascii_encoded = tmp;
    }

  return ret == IDN2_OK ? ascii_encoded : NULL;
}