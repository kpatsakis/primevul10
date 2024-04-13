check_for_verbose_flag(char *line)
{
  char *p = line;
  if (!strcmp(p, "-v")) {
    return 1;
  } else {
    return 0;
  }
}