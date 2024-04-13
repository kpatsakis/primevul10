cmd_spec_header (const char *com, const char *val, void *place_ignored _GL_UNUSED)
{
  /* Empty value means reset the list of headers. */
  if (*val == '\0')
    {
      free_vec (opt.user_headers);
      opt.user_headers = NULL;
      return true;
    }

  if (!check_user_specified_header (val))
    {
      fprintf (stderr, _("%s: %s: Invalid header %s.\n"),
               exec_name, com, quote (val));
      return false;
    }
  opt.user_headers = vec_append (opt.user_headers, val);
  return true;
}