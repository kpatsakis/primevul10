cmd_spec_secure_protocol (const char *com, const char *val, void *place)
{
  static const struct decode_item choices[] = {
    { "auto", secure_protocol_auto },
    { "sslv2", secure_protocol_sslv2 },
    { "sslv3", secure_protocol_sslv3 },
    { "tlsv1", secure_protocol_tlsv1 },
    { "tlsv1_1", secure_protocol_tlsv1_1 },
    { "tlsv1_2", secure_protocol_tlsv1_2 },
    { "pfs", secure_protocol_pfs },
  };
  int ok = decode_string (val, choices, countof (choices), place);
  if (!ok)
    fprintf (stderr, _("%s: %s: Invalid value %s.\n"), exec_name, com, quote (val));
  return ok;
}