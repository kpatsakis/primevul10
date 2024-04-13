void free_string_option(char_u *p)
{
  if (p != empty_option)
    xfree(p);
}