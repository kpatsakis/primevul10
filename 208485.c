char_u *skip_to_option_part(char_u *p)
{
  if (*p == ',') {
    p++;
  }
  while (*p == ' ') {
    p++;
  }
  return p;
}