get_token(void)
{
  unsigned long result;

  if (issue_pointer == 32) {
    /* The lowest number open token has not been returned - bad luck
       to that command client */
    shift_tokens();
  }

  result = token_base + issue_pointer;
  issued_tokens |= (1UL << issue_pointer);
  issue_pointer++;

  return result;
}