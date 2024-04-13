static int optval_default(vimoption_T *p, char_u *varp)
{
  int dvi;

  if (varp == NULL)
    return TRUE;            /* hidden option is always at default */
  dvi = ((p->flags & P_VI_DEF) || p_cp) ? VI_DEFAULT : VIM_DEFAULT;
  if (p->flags & P_NUM)
    return *(long *)varp == (long)p->def_val[dvi];
  if (p->flags & P_BOOL)
    return *(int *)varp == (int)(intptr_t)p->def_val[dvi];
  /* P_STRING */
  return STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0;
}