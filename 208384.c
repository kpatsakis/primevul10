bool is_tty_option(char *name)
{
  return (name[0] == 't' && name[1] == '_') || !strcmp((char *)name, "term");
}