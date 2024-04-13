cmd_spec_verbose (const char *com, const char *val, void *place_ignored _GL_UNUSED)
{
  bool flag;
  if (cmd_boolean (com, val, &flag))
    {
      opt.verbose = flag;
      return true;
    }
  return false;
}