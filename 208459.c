static void check_string_option(char_u **pp)
{
  if (*pp == NULL)
    *pp = empty_option;
}