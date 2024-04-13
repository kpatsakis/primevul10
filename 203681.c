cmd_spec_timeout (const char *com, const char *val, void *place_ignored _GL_UNUSED)
{
  double value;
  if (!cmd_time (com, val, &value))
    return false;
  opt.read_timeout = value;
  opt.connect_timeout = value;
  opt.dns_timeout = value;
  return true;
}