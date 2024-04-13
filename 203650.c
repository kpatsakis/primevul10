cmd_spec_report_speed (const char *com, const char *val, void *place_ignored _GL_UNUSED)
{
  opt.report_bps = strcasecmp (val, "bits") == 0;
  if (!opt.report_bps)
    fprintf (stderr, _("%s: %s: Invalid value %s.\n"), exec_name, com, quote (val));
  return opt.report_bps;
}