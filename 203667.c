cmd_spec_mirror (const char *com, const char *val, void *place_ignored _GL_UNUSED)
{
  int mirror;

  if (!cmd_boolean (com, val, &mirror))
    return false;
  if (mirror)
    {
      opt.recursive = true;
      if (!opt.no_dirstruct)
        opt.dirstruct = true;
      opt.timestamping = true;
      opt.reclevel = INFINITE_RECURSION;
      opt.remove_listing = false;
    }
  return true;
}