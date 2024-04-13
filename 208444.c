int get_option_value_strict(char *name,
                            int64_t *numval,
                            char **stringval,
                            int opt_type,
                            void *from)
{
  if (get_tty_option(name, stringval)) {
    return SOPT_STRING | SOPT_GLOBAL;
  }

  char_u *varp = NULL;
  vimoption_T *p;
  int rv = 0;
  int opt_idx = findoption((uint8_t *)name);
  if (opt_idx < 0) {
    return 0;
  }

  p = &(options[opt_idx]);

  // Hidden option
  if (p->var == NULL) {
    return 0;
  }

  if (p->flags & P_BOOL) {
    rv |= SOPT_BOOL;
  } else if (p->flags & P_NUM) {
    rv |= SOPT_NUM;
  } else if (p->flags & P_STRING) {
    rv |= SOPT_STRING;
  }

  if (p->indir == PV_NONE) {
    if (opt_type == SREQ_GLOBAL)
      rv |= SOPT_GLOBAL;
    else
      return 0; // Did not request global-only option
  } else {
    if (p->indir & PV_BOTH) {
      rv |= SOPT_GLOBAL;
    } else if (opt_type == SREQ_GLOBAL) {
      return 0; // Requested global option
    }

    if (p->indir & PV_WIN) {
      if (opt_type == SREQ_BUF) {
        return 0; // Did not request window-local option
      } else {
        rv |= SOPT_WIN;
      }
    } else if (p->indir & PV_BUF) {
      if (opt_type == SREQ_WIN) {
        return 0; // Did not request buffer-local option
      } else {
        rv |= SOPT_BUF;
      }
    }
  }

  if (stringval == NULL) {
    return rv;
  }

  if (opt_type == SREQ_GLOBAL) {
    varp = p->var;
  } else {
    if (opt_type == SREQ_BUF) {
      // Special case: 'modified' is b_changed, but we also want to
      // consider it set when 'ff' or 'fenc' changed.
      if (p->indir == PV_MOD) {
        *numval = bufIsChanged((buf_T *) from);
        varp = NULL;
      } else {
        aco_save_T	aco;
        aucmd_prepbuf(&aco, (buf_T *) from);
        varp = get_varp(p);
        aucmd_restbuf(&aco);
      }
    } else if (opt_type == SREQ_WIN) {
      win_T	*save_curwin;
      save_curwin = curwin;
      curwin = (win_T *) from;
      curbuf = curwin->w_buffer;
      varp = get_varp(p);
      curwin = save_curwin;
      curbuf = curwin->w_buffer;
    }

    if (varp == p->var) {
      return (rv | SOPT_UNSET);
    }
  }

  if (varp != NULL) {
    if (p->flags & P_STRING) {
      *stringval = xstrdup(*(char **)(varp));
    } else if (p->flags & P_NUM) {
      *numval = *(long *) varp;
    } else {
      *numval = *(int *)varp;
    }
  }

  return rv;
}