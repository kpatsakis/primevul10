shell_is_restricted (name)
     char *name;
{
  char *temp;

  if (restricted)
    return 1;
  temp = base_pathname (name);
  if (*temp == '-')
    temp++;
  return (STREQ (temp, RESTRICTED_SHELL_NAME));
}