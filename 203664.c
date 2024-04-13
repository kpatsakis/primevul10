cmd_cert_type (const char *com, const char *val, void *place)
{
  static const struct decode_item choices[] = {
    { "pem", keyfile_pem },
    { "der", keyfile_asn1 },
    { "asn1", keyfile_asn1 },
  };
  int ok = decode_string (val, choices, countof (choices), place);
  if (!ok)
    fprintf (stderr, _("%s: %s: Invalid value %s.\n"), exec_name, com, quote (val));
  return ok;
}