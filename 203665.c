cmd_spec_prefer_family (const char *com, const char *val, void *place_ignored _GL_UNUSED)
{
  static const struct decode_item choices[] = {
    { "IPv4", prefer_ipv4 },
    { "IPv6", prefer_ipv6 },
    { "none", prefer_none },
  };
  int prefer_family = prefer_none;
  int ok = decode_string (val, choices, countof (choices), &prefer_family);
  if (!ok)
    fprintf (stderr, _("%s: %s: Invalid value %s.\n"), exec_name, com, quote (val));
  opt.prefer_family = prefer_family;
  return ok;
}