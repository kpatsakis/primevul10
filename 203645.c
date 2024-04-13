cmd_spec_recursive (const char *com, const char *val, void *place_ignored _GL_UNUSED)
{
  if (!cmd_boolean (com, val, &opt.recursive))
    return false;
  else
    {
      if (opt.recursive && !opt.no_dirstruct)
        opt.dirstruct = true;
    }
  return true;
}