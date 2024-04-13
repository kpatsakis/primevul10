retrieve_from_file (const char *file, bool html, int *count)
{
  uerr_t status;
  struct urlpos *url_list, *cur_url;
  struct iri *iri = iri_new();

  char *input_file, *url_file = NULL;
  const char *url = file;

  status = RETROK;             /* Suppose everything is OK.  */
  *count = 0;                  /* Reset the URL count.  */

  /* sXXXav : Assume filename and links in the file are in the locale */
  set_uri_encoding (iri, opt.locale, true);
  set_content_encoding (iri, opt.locale);

  if (url_valid_scheme (url))
    {
      int dt,url_err;
      struct url *url_parsed = url_parse (url, &url_err, iri, true);
      if (!url_parsed)
        {
          char *error = url_error (url, url_err);
          logprintf (LOG_NOTQUIET, "%s: %s.\n", url, error);
          xfree (error);
          iri_free (iri);
          return URLERROR;
        }

      if (!opt.base_href)
        opt.base_href = xstrdup (url);

      status = retrieve_url (url_parsed, url, &url_file, NULL, NULL, &dt,
                             false, iri, true);
      url_free (url_parsed);

      if (!url_file || (status != RETROK))
        return status;

      if (dt & TEXTHTML)
        html = true;

#ifdef ENABLE_IRI
      /* If we have a found a content encoding, use it.
       * ( == is okay, because we're checking for identical object) */
      if (iri->content_encoding != opt.locale)
          set_uri_encoding (iri, iri->content_encoding, false);
#endif

      /* Reset UTF-8 encode status */
      iri->utf8_encode = opt.enable_iri;
      xfree (iri->orig_url);

      input_file = url_file;
    }
  else
    input_file = (char *) file;

  url_list = (html ? get_urls_html (input_file, NULL, NULL, iri)
              : get_urls_file (input_file));

  xfree (url_file);

  for (cur_url = url_list; cur_url; cur_url = cur_url->next, ++*count)
    {
      char *filename = NULL, *new_file = NULL, *proxy;
      int dt = 0;
      struct iri *tmpiri = iri_dup (iri);
      struct url *parsed_url = NULL;

      if (cur_url->ignore_when_downloading)
        continue;

      if (opt.quota && total_downloaded_bytes > opt.quota)
        {
          status = QUOTEXC;
          break;
        }

      parsed_url = url_parse (cur_url->url->url, NULL, tmpiri, true);

      proxy = getproxy (cur_url->url);
      if ((opt.recursive || opt.page_requisites)
          && ((cur_url->url->scheme != SCHEME_FTP
#ifdef HAVE_SSL
          && cur_url->url->scheme != SCHEME_FTPS
#endif
          ) || proxy))
        {
          int old_follow_ftp = opt.follow_ftp;

          /* Turn opt.follow_ftp on in case of recursive FTP retrieval */
          if (cur_url->url->scheme == SCHEME_FTP
#ifdef HAVE_SSL
              || cur_url->url->scheme == SCHEME_FTPS
#endif
              )
            opt.follow_ftp = 1;

          status = retrieve_tree (parsed_url ? parsed_url : cur_url->url,
                                  tmpiri);

          opt.follow_ftp = old_follow_ftp;
        }
      else
        status = retrieve_url (parsed_url ? parsed_url : cur_url->url,
                               cur_url->url->url, &filename,
                               &new_file, NULL, &dt, opt.recursive, tmpiri,
                               true);
      xfree (proxy);

      if (parsed_url)
          url_free (parsed_url);

      if (filename && opt.delete_after && file_exists_p (filename, NULL))
        {
          DEBUGP (("\
Removing file due to --delete-after in retrieve_from_file():\n"));
          logprintf (LOG_VERBOSE, _("Removing %s.\n"), filename);
          if (unlink (filename))
            logprintf (LOG_NOTQUIET, "Failed to unlink %s: (%d) %s\n", filename, errno, strerror (errno));
          dt &= ~RETROKF;
        }

      xfree (new_file);
      xfree (filename);
      iri_free (tmpiri);
    }

  /* Free the linked list of URL-s.  */
  free_urlpos (url_list);

  iri_free (iri);

  return status;
}