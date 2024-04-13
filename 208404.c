int check_ff_value(char_u *p)
{
  return check_opt_strings(p, p_ff_values, FALSE);
}