cmd_spec_progress (const char *com, const char *val, void *place_ignored _GL_UNUSED)
{
  if (!valid_progress_implementation_p (val))
    {
      fprintf (stderr, _("%s: %s: Invalid progress type %s.\n"),
               exec_name, com, quote (val));
      return false;
    }
  xfree_null (opt.progress_type);

  /* Don't call set_progress_implementation here.  It will be called
     in main when it becomes clear what the log output is.  */
  opt.progress_type = xstrdup (val);
  return true;
}