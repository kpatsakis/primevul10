void clear_string_option(char_u **pp)
{
  if (*pp != empty_option)
    xfree(*pp);
  *pp = empty_option;
}