metalink_from_http (const struct response *resp, const struct http_stat *hs,
                    const struct url *u)
{
  metalink_t *metalink = NULL;
  metalink_file_t *mfile = xnew0 (metalink_file_t);
  const char *val_beg, *val_end;
  int res_count = 0, meta_count = 0, hash_count = 0, sig_count = 0, i;

  DEBUGP (("Checking for Metalink in HTTP response\n"));

  /* Initialize metalink file for our simple use case.  */
  if (hs->local_file)
    mfile->name = xstrdup (hs->local_file);
  else
    mfile->name = url_file_name (u, NULL);

  /* Begin with 1-element array (for 0-termination). */
  mfile->checksums = xnew0 (metalink_checksum_t *);
  mfile->resources = xnew0 (metalink_resource_t *);
  mfile->metaurls = xnew0 (metalink_metaurl_t *);

  /* Process the Content-Type header.  */
  if (resp_header_locate (resp, "Content-Type", 0, &val_beg, &val_end) != -1)
    {
      metalink_metaurl_t murl = {0};

      const char *type_beg, *type_end;
      char *typestr = NULL;
      char *namestr = NULL;
      size_t type_len;

      DEBUGP (("Processing Content-Type header...\n"));

      /* Find beginning of type.  */
      type_beg = val_beg;
      while (type_beg < val_end && c_isspace (*type_beg))
        type_beg++;

      /* Find end of type.  */
      type_end = type_beg + 1;
      while (type_end < val_end &&
             *type_end != ';' &&
             *type_end != ' ' &&
             *type_end != '\r' &&
             *type_end != '\n')
        type_end++;

      if (type_beg >= val_end || type_end > val_end)
        {
          DEBUGP (("Invalid Content-Type header. Ignoring.\n"));
          goto skip_content_type;
        }

      type_len = type_end - type_beg;
      typestr = xstrndup (type_beg, type_len);

      DEBUGP (("Content-Type: %s\n", typestr));

      if (strcmp (typestr, "application/metalink4+xml"))
        {
          xfree (typestr);
          goto skip_content_type;
        }

      /*
        Valid ranges for the "pri" attribute are from
        1 to 999999.  Mirror servers with a lower value of the "pri"
        attribute have a higher priority, while mirrors with an undefined
        "pri" attribute are considered to have a value of 999999, which is
        the lowest priority.

        rfc6249 section 3.1
      */
      murl.priority = DEFAULT_PRI;

      murl.mediatype = typestr;
      typestr = NULL;

      if (opt.content_disposition
          && resp_header_locate (resp, "Content-Disposition", 0, &val_beg, &val_end) != -1)
        {
          find_key_value (val_beg, val_end, "filename", &namestr);
          murl.name = namestr;
          namestr = NULL;
        }

      murl.url = xstrdup (u->url);

      DEBUGP (("URL=%s\n", murl.url));
      DEBUGP (("MEDIATYPE=%s\n", murl.mediatype));
      DEBUGP (("NAME=%s\n", murl.name ? murl.name : ""));
      DEBUGP (("PRIORITY=%d\n", murl.priority));

      /* 1 slot from new resource, 1 slot for null-termination.  */
      mfile->metaurls = xrealloc (mfile->metaurls,
                                  sizeof (metalink_metaurl_t *) * (meta_count + 2));
      mfile->metaurls[meta_count] = xnew0 (metalink_metaurl_t);
      *mfile->metaurls[meta_count] = murl;
      meta_count++;
    }
skip_content_type:

  /* Find all Link headers.  */
  for (i = 0;
       (i = resp_header_locate (resp, "Link", i, &val_beg, &val_end)) != -1;
       i++)
    {
      char *rel = NULL, *reltype = NULL;
      char *urlstr = NULL;
      const char *url_beg, *url_end, *attrs_beg;
      size_t url_len;

      /* Sample Metalink Link headers:

           Link: <http://www2.example.com/dir1/dir2/dir3/dir4/dir5/example.ext>;
           rel=duplicate; pri=1; pref; geo=gb; depth=4

           Link: <http://example.com/example.ext.asc>; rel=describedby;
           type="application/pgp-signature"
       */

      /* Find beginning of URL.  */
      url_beg = val_beg;
      while (url_beg < val_end - 1 && c_isspace (*url_beg))
        url_beg++;

      /* Find end of URL.  */
      /* The convention here is that end ptr points to one element after
         end of string. In this case, it should be pointing to the '>', which
         is one element after end of actual URL. Therefore, it should never point
         to val_end, which is one element after entire header value string.  */
      url_end = url_beg + 1;
      while (url_end < val_end - 1 && *url_end != '>')
        url_end++;

      if (url_beg >= val_end || url_end >= val_end ||
          *url_beg != '<' || *url_end != '>')
        {
          DEBUGP (("This is not a valid Link header. Ignoring.\n"));
          continue;
        }

      /* Skip <.  */
      url_beg++;
      url_len = url_end - url_beg;

      /* URL found. Now handle the attributes.  */
      attrs_beg = url_end + 1;

      /* First we need to find out what type of link it is. Currently, we
         support rel=duplicate and rel=describedby.  */
      if (!find_key_value (attrs_beg, val_end, "rel", &rel))
        {
          DEBUGP (("No rel value in Link header, skipping.\n"));
          continue;
        }

      urlstr = xstrndup (url_beg, url_len);
      DEBUGP (("URL=%s\n", urlstr));
      DEBUGP (("rel=%s\n", rel));

      if (!strcmp (rel, "describedby"))
        find_key_value (attrs_beg, val_end, "type", &reltype);

      /* Handle signatures.
         Libmetalink only supports one signature per file. Therefore we stop
         as soon as we successfully get first supported signature.  */
      if (sig_count == 0 &&
          reltype && !strcmp (reltype, "application/pgp-signature"))
        {
          /* Download the signature to a temporary file.  */
          FILE *_output_stream = output_stream;
          bool _output_stream_regular = output_stream_regular;

          output_stream = tmpfile ();
          if (output_stream)
            {
              struct iri *iri = iri_new ();
              struct url *url;
              int url_err;

              set_uri_encoding (iri, opt.locale, true);
              url = url_parse (urlstr, &url_err, iri, false);

              if (!url)
                {
                  char *error = url_error (urlstr, url_err);
                  logprintf (LOG_NOTQUIET, _("When downloading signature:\n"
                                             "%s: %s.\n"), urlstr, error);
                  xfree (error);
                  iri_free (iri);
                }
              else
                {
                  /* Avoid recursive Metalink from HTTP headers.  */
                  bool _metalink_http = opt.metalink_over_http;
                  uerr_t retr_err;

                  opt.metalink_over_http = false;
                  retr_err = retrieve_url (url, urlstr, NULL, NULL,
                                           NULL, NULL, false, iri, false);
                  opt.metalink_over_http = _metalink_http;

                  url_free (url);
                  iri_free (iri);

                  if (retr_err == RETROK)
                    {
                      /* Signature is in the temporary file. Read it into
                         metalink resource structure.  */
                      metalink_signature_t msig;
                      size_t siglen;

                      fseek (output_stream, 0, SEEK_END);
                      siglen = ftell (output_stream);
                      fseek (output_stream, 0, SEEK_SET);

                      DEBUGP (("siglen=%lu\n", siglen));

                      msig.signature = xmalloc (siglen + 1);
                      if (fread (msig.signature, siglen, 1, output_stream) != 1)
                        {
                          logputs (LOG_NOTQUIET,
                                   _("Unable to read signature content from "
                                     "temporary file. Skipping.\n"));
                          xfree (msig.signature);
                        }
                      else
                        {
                          msig.signature[siglen] = '\0'; /* Just in case.  */
                          msig.mediatype = xstrdup ("application/pgp-signature");

                          DEBUGP (("Signature (%s):\n%s\n",
                                   msig.mediatype, msig.signature));

                          mfile->signature = xnew (metalink_signature_t);
                          *mfile->signature = msig;

                          sig_count++;
                        }
                    }
                }
              fclose (output_stream);
            }
          else
            {
              logputs (LOG_NOTQUIET, _("Could not create temporary file. "
                                       "Skipping signature download.\n"));
            }
          output_stream_regular = _output_stream_regular;
          output_stream = _output_stream;
        } /* Iterate over signatures.  */

        /* Handle Metalink resources.  */
      else if (!strcmp (rel, "duplicate"))
        {
          metalink_resource_t mres = {0};
          char *pristr;

          /*
             Valid ranges for the "pri" attribute are from
             1 to 999999.  Mirror servers with a lower value of the "pri"
             attribute have a higher priority, while mirrors with an undefined
             "pri" attribute are considered to have a value of 999999, which is
             the lowest priority.

             rfc6249 section 3.1
           */
          mres.priority = DEFAULT_PRI;
          if (find_key_value (url_end, val_end, "pri", &pristr))
            {
              long pri;
              char *end_pristr;
              /* Do not care for errno since 0 is error in this case.  */
              pri = strtol (pristr, &end_pristr, 10);
              if (end_pristr != pristr + strlen (pristr) ||
                  !VALID_PRI_RANGE (pri))
                {
                  /* This is against the specification, so let's inform the user.  */
                  logprintf (LOG_NOTQUIET,
                             _("Invalid pri value. Assuming %d.\n"),
                             DEFAULT_PRI);
                }
              else
                mres.priority = pri;
              xfree (pristr);
            }

          switch (url_scheme (urlstr))
            {
            case SCHEME_HTTP:
              mres.type = xstrdup ("http");
              break;
#ifdef HAVE_SSL
            case SCHEME_HTTPS:
              mres.type = xstrdup ("https");
              break;
            case SCHEME_FTPS:
              mres.type = xstrdup ("ftps");
              break;
#endif
            case SCHEME_FTP:
              mres.type = xstrdup ("ftp");
              break;
            default:
              DEBUGP (("Unsupported url scheme in %s. Skipping resource.\n", urlstr));
            }

          if (mres.type)
            {
              DEBUGP (("TYPE=%s\n", mres.type));

              /* At this point we have validated the new resource.  */

              find_key_value (url_end, val_end, "geo", &mres.location);

              mres.url = urlstr;
              urlstr = NULL;

              mres.preference = 0;
              if (has_key (url_end, val_end, "pref"))
                {
                  DEBUGP (("This resource has preference\n"));
                  mres.preference = 1;
                }

              /* 1 slot from new resource, 1 slot for null-termination.  */
              mfile->resources = xrealloc (mfile->resources,
                                           sizeof (metalink_resource_t *) * (res_count + 2));
              mfile->resources[res_count] = xnew0 (metalink_resource_t);
              *mfile->resources[res_count] = mres;
              res_count++;
            }
        } /* Handle resource link (rel=duplicate).  */

      /* Handle Metalink/XML resources.  */
      else if (reltype && !strcmp (reltype, "application/metalink4+xml"))
        {
          metalink_metaurl_t murl = {0};
          char *pristr;

          /*
             Valid ranges for the "pri" attribute are from
             1 to 999999.  Mirror servers with a lower value of the "pri"
             attribute have a higher priority, while mirrors with an undefined
             "pri" attribute are considered to have a value of 999999, which is
             the lowest priority.

             rfc6249 section 3.1
           */
          murl.priority = DEFAULT_PRI;
          if (find_key_value (url_end, val_end, "pri", &pristr))
            {
              long pri;
              char *end_pristr;
              /* Do not care for errno since 0 is error in this case.  */
              pri = strtol (pristr, &end_pristr, 10);
              if (end_pristr != pristr + strlen (pristr) ||
                  !VALID_PRI_RANGE (pri))
                {
                  /* This is against the specification, so let's inform the user.  */
                  logprintf (LOG_NOTQUIET,
                             _("Invalid pri value. Assuming %d.\n"),
                             DEFAULT_PRI);
                }
              else
                murl.priority = pri;
              xfree (pristr);
            }

          murl.mediatype = xstrdup (reltype);

          DEBUGP (("MEDIATYPE=%s\n", murl.mediatype));

          /* At this point we have validated the new resource.  */

          find_key_value (url_end, val_end, "name", &murl.name);

          murl.url = urlstr;
          urlstr = NULL;

          /* 1 slot from new resource, 1 slot for null-termination.  */
          mfile->metaurls = xrealloc (mfile->metaurls,
                                       sizeof (metalink_metaurl_t *) * (meta_count + 2));
          mfile->metaurls[meta_count] = xnew0 (metalink_metaurl_t);
          *mfile->metaurls[meta_count] = murl;
          meta_count++;
        } /* Handle resource link (rel=describedby).  */
      else
        DEBUGP (("This link header was not used for Metalink\n"));

      xfree (urlstr);
      xfree (reltype);
      xfree (rel);
    } /* Iterate over link headers.  */

  /* Null-terminate resources array.  */
  mfile->resources[res_count] = 0;
  mfile->metaurls[meta_count] = 0;

  if (res_count == 0 && meta_count == 0)
    {
      DEBUGP (("No valid metalink references found.\n"));
      goto fail;
    }

  /* Find all Digest headers.  */
  for (i = 0;
       (i = resp_header_locate (resp, "Digest", i, &val_beg, &val_end)) != -1;
       i++)
    {
      const char *dig_pos;
      char *dig_type, *dig_hash;

      /* Each Digest header can include multiple hashes. Example:
           Digest: SHA=thvDyvhfIqlvFe+A9MYgxAfm1q5=,unixsum=30637
           Digest: md5=HUXZLQLMuI/KZ5KDcJPcOA==
       */
      for (dig_pos = val_beg;
           (dig_pos = find_key_values (dig_pos, val_end, &dig_type, &dig_hash));
           dig_pos++)
        {
          /* The hash here is assumed to be base64. We need the hash in hex.
             Therefore we convert: base64 -> binary -> hex.  */
          const size_t dig_hash_str_len = strlen (dig_hash);
          char *bin_hash = alloca (dig_hash_str_len * 3 / 4 + 1);
          ssize_t hash_bin_len;

          hash_bin_len = wget_base64_decode (dig_hash, bin_hash, dig_hash_str_len * 3 / 4 + 1);

          /* Detect malformed base64 input.  */
          if (hash_bin_len < 0)
            {
              xfree (dig_type);
              xfree (dig_hash);
              continue;
            }

          /* One slot for me, one for zero-termination.  */
          mfile->checksums =
                  xrealloc (mfile->checksums,
                            sizeof (metalink_checksum_t *) * (hash_count + 2));
          mfile->checksums[hash_count] = xnew (metalink_checksum_t);
          mfile->checksums[hash_count]->type = dig_type;

          mfile->checksums[hash_count]->hash = xmalloc ((size_t)hash_bin_len * 2 + 1);
          wg_hex_to_string (mfile->checksums[hash_count]->hash, bin_hash, (size_t)hash_bin_len);

          xfree (dig_hash);

          hash_count++;
        }
    }

  /* Zero-terminate checksums array.  */
  mfile->checksums[hash_count] = 0;

  /*
    If Instance Digests are not provided by the Metalink servers, the
    Link header fields pertaining to this specification MUST be ignored.

    rfc6249 section 6
   */
  if (res_count && hash_count == 0)
    {
      logputs (LOG_VERBOSE,
               _("Could not find acceptable digest for Metalink resources.\n"
                 "Ignoring them.\n"));
      goto fail;
    }

  /* Metalink data is OK. Now we just need to sort the resources based
     on their priorities, preference, and perhaps location.  */
  stable_sort (mfile->resources, res_count, sizeof (metalink_resource_t *), metalink_res_cmp);
  stable_sort (mfile->metaurls, meta_count, sizeof (metalink_metaurl_t *), metalink_meta_cmp);

  /* Restore sensible preference values (in case someone cares to look).  */
  for (i = 0; i < res_count; ++i)
    mfile->resources[i]->preference = 1000000 - mfile->resources[i]->priority;

  metalink = xnew0 (metalink_t);
  metalink->files = xmalloc (sizeof (metalink_file_t *) * 2);
  metalink->files[0] = mfile;
  metalink->files[1] = 0;
  metalink->origin = xstrdup (u->url);
  metalink->version = METALINK_VERSION_4;
  /* Leave other fields set to 0.  */

  return metalink;

fail:
  /* Free all allocated memory.  */
  if (metalink)
    metalink_delete (metalink);
  else
    metalink_file_delete (mfile);
  return NULL;
}