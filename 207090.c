send_append_header2 (char const *name, char const *value, int mode)
{
  add_header (mu_strdup (name), mu_strdup (value), mode);
}