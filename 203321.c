check_token(unsigned long token)
{
  int result;
  unsigned long pos;

  if (token < token_base) {
    /* Token too old */
    result = 0;
  } else {
    pos = token - token_base;
    if (pos >= issue_pointer) {
      /* Token hasn't been issued yet */
      result = 0;
    } else {
      if (returned_tokens & (1UL << pos)) {
        /* Token has already been returned */
        result = 0;
      } else {
        /* Token is OK */
        result = 1;
        returned_tokens |= (1UL << pos);
        if (pos == 0) {
          shift_tokens();
        }
      }
    }
  }

  return result;

}