void set_helplang_default(const char *lang)
{
  int idx;

  if (lang == NULL || STRLEN(lang) < 2)         /* safety check */
    return;
  idx = findoption((char_u *)"hlg");
  if (idx >= 0 && !(options[idx].flags & P_WAS_SET)) {
    if (options[idx].flags & P_ALLOCED)
      free_string_option(p_hlg);
    p_hlg = (char_u *)xstrdup(lang);
    /* zh_CN becomes "cn", zh_TW becomes "tw". */
    if (STRNICMP(p_hlg, "zh_", 3) == 0 && STRLEN(p_hlg) >= 5) {
      p_hlg[0] = (char_u)TOLOWER_ASC(p_hlg[3]);
      p_hlg[1] = (char_u)TOLOWER_ASC(p_hlg[4]);
    }
    p_hlg[2] = NUL;
    options[idx].flags |= P_ALLOCED;
  }
}