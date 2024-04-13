cmd_spec_warc_header (const char *com, const char *val, void *place_ignored _GL_UNUSED)
{
  /* Empty value means reset the list of headers. */
  if (*val == '\0')
    {
      free_vec (opt.warc_user_headers);
      opt.warc_user_headers = NULL;
      return true;
    }

  if (!check_user_specified_header (val))
    {
      fprintf (stderr, _("%s: %s: Invalid WARC header %s.\n"),
               exec_name, com, quote (val));
      return false;
    }
  opt.warc_user_headers = vec_append (opt.warc_user_headers, val);
  return true;
}