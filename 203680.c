test_commands_sorted(void)
{
  unsigned i;

  for (i = 1; i < countof(commands); ++i)
    {
      if (strcasecmp (commands[i - 1].name, commands[i].name) > 0)
        {
          mu_assert ("FAILED", false);
          break;
        }
    }
  return NULL;
}