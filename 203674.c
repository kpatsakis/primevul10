setval_internal (int comind, const char *com, const char *val)
{
  assert (0 <= comind && ((size_t) comind) < countof (commands));
  DEBUGP (("Setting %s (%s) to %s\n", com, commands[comind].name, val));
  return commands[comind].action (com, val, commands[comind].place);
}