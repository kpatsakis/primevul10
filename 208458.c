char_u *find_shada_parameter(int type)
{
  char_u  *p;

  for (p = p_shada; *p; ++p) {
    if (*p == type)
      return p + 1;
    if (*p == 'n')                  /* 'n' is always the last one */
      break;
    p = vim_strchr(p, ',');         /* skip until next ',' */
    if (p == NULL)                  /* hit the end without finding parameter */
      break;
  }
  return NULL;
}