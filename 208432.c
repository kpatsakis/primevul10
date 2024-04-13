static bool valid_filetype(char_u *val)
{
  for (char_u *s = val; *s != NUL; s++) {
    if (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)".-_", *s) == NULL) {
      return false;
    }
  }
  return true;
}