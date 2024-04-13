int ExpandSettings(expand_T *xp, regmatch_T *regmatch, int *num_file, char_u ***file)
{
  int num_normal = 0;  // Nr of matching non-term-code settings
  int match;
  int count = 0;
  char_u      *str;
  int loop;
  static char *(names[]) = {"all", "termcap"};
  int ic = regmatch->rm_ic;             /* remember the ignore-case flag */

  /* do this loop twice:
   * loop == 0: count the number of matching options
   * loop == 1: copy the matching options into allocated memory
   */
  for (loop = 0; loop <= 1; ++loop) {
    regmatch->rm_ic = ic;
    if (xp->xp_context != EXPAND_BOOL_SETTINGS) {
      for (match = 0; match < (int)ARRAY_SIZE(names);
           ++match)
        if (vim_regexec(regmatch, (char_u *)names[match], (colnr_T)0)) {
          if (loop == 0)
            num_normal++;
          else
            (*file)[count++] = vim_strsave((char_u *)names[match]);
        }
    }
    for (size_t opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != NULL;
         opt_idx++) {
      if (options[opt_idx].var == NULL)
        continue;
      if (xp->xp_context == EXPAND_BOOL_SETTINGS
          && !(options[opt_idx].flags & P_BOOL))
        continue;
      match = FALSE;
      if (vim_regexec(regmatch, str, (colnr_T)0)
          || (options[opt_idx].shortname != NULL
              && vim_regexec(regmatch,
                  (char_u *)options[opt_idx].shortname, (colnr_T)0))){
        match = TRUE;
      }

      if (match) {
        if (loop == 0) {
          num_normal++;
        } else
          (*file)[count++] = vim_strsave(str);
      }
    }

    if (loop == 0) {
      if (num_normal > 0) {
        *num_file = num_normal;
      } else {
        return OK;
      }
      *file = (char_u **)xmalloc((size_t)(*num_file) * sizeof(char_u *));
    }
  }
  return OK;
}