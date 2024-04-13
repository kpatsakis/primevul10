cmd_spec_htmlify (const char *com, const char *val, void *place_ignored _GL_UNUSED)
{
  int flag = cmd_boolean (com, val, &opt.htmlify);
  if (flag && !opt.htmlify)
    opt.remove_listing = false;
  return flag;
}