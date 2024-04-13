static char_u *did_set_spell_option(bool is_spellfile)
{
  char_u  *errmsg = NULL;

  if (is_spellfile) {
    int l = (int)STRLEN(curwin->w_s->b_p_spf);
    if (l > 0
        && (l < 4 || STRCMP(curwin->w_s->b_p_spf + l - 4, ".add") != 0)) {
      errmsg = e_invarg;
    }
  }

  if (errmsg == NULL) {
    FOR_ALL_WINDOWS_IN_TAB(wp, curtab) {
      if (wp->w_buffer == curbuf && wp->w_p_spell) {
        errmsg = did_set_spelllang(wp);
        break;
      }
    }
  }

  return errmsg;
}