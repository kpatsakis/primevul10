uint get_errcode_from_name(const char *error_name, const char *error_end)
{
  uint tmp;
  if ((tmp= get_errcode_from_name(error_name, error_end,
                                     global_error_names)))
    return tmp;
  if ((tmp= get_errcode_from_name(error_name, error_end,
                                     handler_error_names)))
    return tmp;
  die("Unknown SQL error name '%s'", error_name);
}