ensure_extension (struct http_stat *hs, const char *ext, int *dt)
{
  char *last_period_in_local_filename = strrchr (hs->local_file, '.');
  char shortext[8];
  int len;
  shortext[0] = '\0';
  len = strlen (ext);
  if (len == 5)
    {
      memcpy (shortext, ext, len - 1);
      shortext[len - 1] = '\0';
    }

  if (last_period_in_local_filename == NULL
      || !(0 == strcasecmp (last_period_in_local_filename, shortext)
           || 0 == strcasecmp (last_period_in_local_filename, ext)))
    {
      int local_filename_len = strlen (hs->local_file);
      /* Resize the local file, allowing for ".html" preceded by
         optional ".NUMBER".  */
      hs->local_file = xrealloc (hs->local_file,
                                 local_filename_len + 24 + len);
      strcpy (hs->local_file + local_filename_len, ext);
      /* If clobbering is not allowed and the file, as named,
         exists, tack on ".NUMBER.html" instead. */
      if (!ALLOW_CLOBBER && file_exists_p (hs->local_file, NULL))
        {
          int ext_num = 1;
          do
            sprintf (hs->local_file + local_filename_len,
                     ".%d%s", ext_num++, ext);
          while (file_exists_p (hs->local_file, NULL));
        }
      *dt |= ADDED_HTML_EXTENSION;
    }
}