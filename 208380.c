option_value2string (
    vimoption_T *opp,
    int opt_flags                          /* OPT_GLOBAL and/or OPT_LOCAL */
)
{
  char_u      *varp;

  varp = get_varp_scope(opp, opt_flags);

  if (opp->flags & P_NUM) {
    long wc = 0;

    if (wc_use_keyname(varp, &wc)) {
      STRLCPY(NameBuff, get_special_key_name((int)wc, 0), sizeof(NameBuff));
    } else if (wc != 0) {
      STRLCPY(NameBuff, transchar((int)wc), sizeof(NameBuff));
    } else {
      snprintf((char *)NameBuff,
               sizeof(NameBuff),
               "%" PRId64,
               (int64_t)*(long *)varp);
    }
  } else {  // P_STRING
    varp = *(char_u **)(varp);
    if (varp == NULL)                       /* just in case */
      NameBuff[0] = NUL;
    else if (opp->flags & P_EXPAND)
      home_replace(NULL, varp, NameBuff, MAXPATHL, FALSE);
    /* Translate 'pastetoggle' into special key names */
    else if ((char_u **)opp->var == &p_pt)
      str2specialbuf(p_pt, NameBuff, MAXPATHL);
    else
      STRLCPY(NameBuff, varp, MAXPATHL);
  }
}