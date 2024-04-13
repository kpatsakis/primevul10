shift_tokens(void)
{
  do {
    issued_tokens >>= 1;
    returned_tokens >>= 1;
    token_base++;
    issue_pointer--;
  } while ((issued_tokens & 1) && (returned_tokens & 1));
}