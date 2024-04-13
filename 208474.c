char_u *get_equalprg(void)
{
  if (*curbuf->b_p_ep == NUL)
    return p_ep;
  return curbuf->b_p_ep;
}