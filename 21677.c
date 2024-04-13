completion_glob_pattern (string)
     char *string;
{
  return (glob_pattern_p (string) == 1);
}