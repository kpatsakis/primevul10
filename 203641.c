cleanup (void)
{
  /* Free external resources, close files, etc. */

  /* Close WARC file. */
  if (opt.warc_filename != 0)
    warc_close ();

  log_close ();

  if (output_stream)
    if (fclose (output_stream) == EOF)
      inform_exit_status (CLOSEFAILED);

  /* No need to check for error because Wget flushes its output (and
     checks for errors) after any data arrives.  */

  /* We're exiting anyway so there's no real need to call free()
     hundreds of times.  Skipping the frees will make Wget exit
     faster.

     However, when detecting leaks, it's crucial to free() everything
     because then you can find the real leaks, i.e. the allocated
     memory which grows with the size of the program.  */

#ifdef DEBUG_MALLOC
  convert_cleanup ();
  res_cleanup ();
  http_cleanup ();
  cleanup_html_url ();
  spider_cleanup ();
  host_cleanup ();
  log_cleanup ();
  netrc_cleanup (netrc_list);

  for (i = 0; i < nurl; i++)
    xfree (url[i]);

  xfree_null (opt.choose_config);
  xfree_null (opt.lfilename);
  xfree_null (opt.dir_prefix);
  xfree_null (opt.input_filename);
  xfree_null (opt.output_document);
  free_vec (opt.accepts);
  free_vec (opt.rejects);
  free_vec (opt.excludes);
  free_vec (opt.includes);
  free_vec (opt.domains);
  free_vec (opt.follow_tags);
  free_vec (opt.ignore_tags);
  xfree_null (opt.progress_type);
  xfree_null (opt.ftp_user);
  xfree_null (opt.ftp_passwd);
  xfree_null (opt.ftp_proxy);
  xfree_null (opt.https_proxy);
  xfree_null (opt.http_proxy);
  free_vec (opt.no_proxy);
  xfree_null (opt.useragent);
  xfree_null (opt.referer);
  xfree_null (opt.http_user);
  xfree_null (opt.http_passwd);
  free_vec (opt.user_headers);
  free_vec (opt.warc_user_headers);
# ifdef HAVE_SSL
  xfree_null (opt.cert_file);
  xfree_null (opt.private_key);
  xfree_null (opt.ca_directory);
  xfree_null (opt.ca_cert);
  xfree_null (opt.random_file);
  xfree_null (opt.egd_file);
# endif
  xfree_null (opt.bind_address);
  xfree_null (opt.cookies_input);
  xfree_null (opt.cookies_output);
  xfree_null (opt.user);
  xfree_null (opt.passwd);
  xfree_null (opt.base_href);
  xfree_null (opt.method);

#endif /* DEBUG_MALLOC */
}