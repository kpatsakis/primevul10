send_append_header (char const *text)
{
  char *p;
  size_t len;
  char *name;

  p = strchr (text, ':');
  if (!p)
    {
      mu_error (_("Invalid header: %s"), text);
      return;
    }
  len = p - text;
  name = mu_alloc (len + 1);
  memcpy (name, text, len);
  name[len] = 0;
  for (p++; *p && mu_isspace (*p); p++)
    ;

  add_header (name, mu_strdup (p), COMPOSE_APPEND);
}