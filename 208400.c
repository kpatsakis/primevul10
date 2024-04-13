set_num_option (
    int opt_idx,                            /* index in options[] table */
    char_u *varp,                      /* pointer to the option variable */
    long value,                             /* new value */
    char_u *errbuf,                    /* buffer for error messages */
    size_t errbuflen,                       /* length of "errbuf" */
    int opt_flags                          /* OPT_LOCAL, OPT_GLOBAL and
                                           OPT_MODELINE */
)
{
  char_u      *errmsg = NULL;
  long old_value = *(long *)varp;
  long old_Rows = Rows;                 /* remember old Rows */
  long old_Columns = Columns;           /* remember old Columns */
  long        *pp = (long *)varp;

  /* Disallow changing some options from secure mode. */
  if ((secure || sandbox != 0)
      && (options[opt_idx].flags & P_SECURE)) {
    return e_secure;
  }

  *pp = value;
  /* Remember where the option was set. */
  set_option_scriptID_idx(opt_idx, opt_flags, current_SID);

  if (curbuf->b_p_sw < 0) {
    errmsg = e_positive;
    curbuf->b_p_sw = curbuf->b_p_ts;
  }

  /*
   * Number options that need some action when changed
   */
  if (pp == &p_wh || pp == &p_hh) {
    if (p_wh < 1) {
      errmsg = e_positive;
      p_wh = 1;
    }
    if (p_wmh > p_wh) {
      errmsg = e_winheight;
      p_wh = p_wmh;
    }
    if (p_hh < 0) {
      errmsg = e_positive;
      p_hh = 0;
    }

    /* Change window height NOW */
    if (lastwin != firstwin) {
      if (pp == &p_wh && curwin->w_height < p_wh)
        win_setheight((int)p_wh);
      if (pp == &p_hh && curbuf->b_help && curwin->w_height < p_hh)
        win_setheight((int)p_hh);
    }
  }
  /* 'winminheight' */
  else if (pp == &p_wmh) {
    if (p_wmh < 0) {
      errmsg = e_positive;
      p_wmh = 0;
    }
    if (p_wmh > p_wh) {
      errmsg = e_winheight;
      p_wmh = p_wh;
    }
    win_setminheight();
  } else if (pp == &p_wiw) {
    if (p_wiw < 1) {
      errmsg = e_positive;
      p_wiw = 1;
    }
    if (p_wmw > p_wiw) {
      errmsg = e_winwidth;
      p_wiw = p_wmw;
    }

    /* Change window width NOW */
    if (lastwin != firstwin && curwin->w_width < p_wiw)
      win_setwidth((int)p_wiw);
  }
  /* 'winminwidth' */
  else if (pp == &p_wmw) {
    if (p_wmw < 0) {
      errmsg = e_positive;
      p_wmw = 0;
    }
    if (p_wmw > p_wiw) {
      errmsg = e_winwidth;
      p_wmw = p_wiw;
    }
    win_setminheight();
  } else if (pp == &p_ls) {
    /* (re)set last window status line */
    last_status(false);
  }
  /* (re)set tab page line */
  else if (pp == &p_stal) {
    shell_new_rows();           /* recompute window positions and heights */
  }
  /* 'foldlevel' */
  else if (pp == &curwin->w_p_fdl) {
    if (curwin->w_p_fdl < 0)
      curwin->w_p_fdl = 0;
    newFoldLevel();
  }
  /* 'foldminlines' */
  else if (pp == &curwin->w_p_fml) {
    foldUpdateAll(curwin);
  }
  /* 'foldnestmax' */
  else if (pp == &curwin->w_p_fdn) {
    if (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
      foldUpdateAll(curwin);
  }
  /* 'foldcolumn' */
  else if (pp == &curwin->w_p_fdc) {
    if (curwin->w_p_fdc < 0) {
      errmsg = e_positive;
      curwin->w_p_fdc = 0;
    } else if (curwin->w_p_fdc > 12) {
      errmsg = e_invarg;
      curwin->w_p_fdc = 12;
    }
  // 'shiftwidth' or 'tabstop'
  } else if (pp == &curbuf->b_p_sw || pp == (long *)&curbuf->b_p_ts) {
    if (foldmethodIsIndent(curwin)) {
      foldUpdateAll(curwin);
    }
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    if (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0) {
      parse_cino(curbuf);
    }
  }
  /* 'maxcombine' */
  else if (pp == &p_mco) {
    if (p_mco > MAX_MCO)
      p_mco = MAX_MCO;
    else if (p_mco < 0)
      p_mco = 0;
    screenclear();          /* will re-allocate the screen */
  } else if (pp == &curbuf->b_p_iminsert) {
    if (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_LAST) {
      errmsg = e_invarg;
      curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    showmode();
    /* Show/unshow value of 'keymap' in status lines. */
    status_redraw_curbuf();
  } else if (pp == &p_window) {
    if (p_window < 1)
      p_window = 1;
    else if (p_window >= Rows)
      p_window = Rows - 1;
  } else if (pp == &curbuf->b_p_imsearch) {
    if (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_LAST) {
      errmsg = e_invarg;
      curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;
  }
  /* if 'titlelen' has changed, redraw the title */
  else if (pp == &p_titlelen) {
    if (p_titlelen < 0) {
      errmsg = e_positive;
      p_titlelen = 85;
    }
    if (starting != NO_SCREEN && old_value != p_titlelen)
      need_maketitle = TRUE;
  }
  /* if p_ch changed value, change the command line height */
  else if (pp == &p_ch) {
    if (p_ch < 1) {
      errmsg = e_positive;
      p_ch = 1;
    }
    if (p_ch > Rows - min_rows() + 1)
      p_ch = Rows - min_rows() + 1;

    /* Only compute the new window layout when startup has been
     * completed. Otherwise the frame sizes may be wrong. */
    if (p_ch != old_value && full_screen
        )
      command_height();
  }
  /* when 'updatecount' changes from zero to non-zero, open swap files */
  else if (pp == &p_uc) {
    if (p_uc < 0) {
      errmsg = e_positive;
      p_uc = 100;
    }
    if (p_uc && !old_value)
      ml_open_files();
  } else if (pp == &curwin->w_p_cole) {
    if (curwin->w_p_cole < 0) {
      errmsg = e_positive;
      curwin->w_p_cole = 0;
    } else if (curwin->w_p_cole > 3) {
      errmsg = e_invarg;
      curwin->w_p_cole = 3;
    }
  }
  /* sync undo before 'undolevels' changes */
  else if (pp == &p_ul) {
    /* use the old value, otherwise u_sync() may not work properly */
    p_ul = old_value;
    u_sync(TRUE);
    p_ul = value;
  } else if (pp == &curbuf->b_p_ul) {
    /* use the old value, otherwise u_sync() may not work properly */
    curbuf->b_p_ul = old_value;
    u_sync(TRUE);
    curbuf->b_p_ul = value;
  }
  /* 'numberwidth' must be positive */
  else if (pp == &curwin->w_p_nuw) {
    if (curwin->w_p_nuw < 1) {
      errmsg = e_positive;
      curwin->w_p_nuw = 1;
    }
    if (curwin->w_p_nuw > 10) {
      errmsg = e_invarg;
      curwin->w_p_nuw = 10;
    }
    curwin->w_nrwidth_line_count = 0;
  } else if (pp == &curbuf->b_p_tw) {
    if (curbuf->b_p_tw < 0) {
      errmsg = e_positive;
      curbuf->b_p_tw = 0;
    }

    FOR_ALL_TAB_WINDOWS(tp, wp) {
      check_colorcolumn(wp);
    }

  }

  /*
   * Check the bounds for numeric options here
   */
  if (Rows < min_rows() && full_screen) {
    if (errbuf != NULL) {
      vim_snprintf((char *)errbuf, errbuflen,
          _("E593: Need at least %d lines"), min_rows());
      errmsg = errbuf;
    }
    Rows = min_rows();
  }
  if (Columns < MIN_COLUMNS && full_screen) {
    if (errbuf != NULL) {
      vim_snprintf((char *)errbuf, errbuflen,
          _("E594: Need at least %d columns"), MIN_COLUMNS);
      errmsg = errbuf;
    }
    Columns = MIN_COLUMNS;
  }
  limit_screen_size();


  /*
   * If the screen (shell) height has been changed, assume it is the
   * physical screenheight.
   */
  if (old_Rows != Rows || old_Columns != Columns) {
    /* Changing the screen size is not allowed while updating the screen. */
    if (updating_screen) {
      *pp = old_value;
    } else if (full_screen) {
      screen_resize((int)Columns, (int)Rows);
    } else {
      /* Postpone the resizing; check the size and cmdline position for
       * messages. */
      check_shellsize();
      if (cmdline_row > Rows - p_ch && Rows > p_ch) {
        assert(p_ch >= 0 && Rows - p_ch <= INT_MAX);
        cmdline_row = (int)(Rows - p_ch);
      }
    }
    if (p_window >= Rows || !option_was_set((char_u *)"window"))
      p_window = Rows - 1;
  }

  if (curbuf->b_p_ts <= 0) {
    errmsg = e_positive;
    curbuf->b_p_ts = 8;
  }
  if (p_tm < 0) {
    errmsg = e_positive;
    p_tm = 0;
  }
  if ((curwin->w_p_scr <= 0
       || (curwin->w_p_scr > curwin->w_height
           && curwin->w_height > 0))
      && full_screen) {
    if (pp == &(curwin->w_p_scr)) {
      if (curwin->w_p_scr != 0)
        errmsg = e_scroll;
      win_comp_scroll(curwin);
    }
    /* If 'scroll' became invalid because of a side effect silently adjust
     * it. */
    else if (curwin->w_p_scr <= 0)
      curwin->w_p_scr = 1;
    else     /* curwin->w_p_scr > curwin->w_height */
      curwin->w_p_scr = curwin->w_height;
  }
  if (p_hi < 0) {
    errmsg = e_positive;
    p_hi = 0;
  } else if (p_hi > 10000) {
    errmsg = e_invarg;
    p_hi = 10000;
  }
  if (p_re < 0 || p_re > 2) {
    errmsg = e_invarg;
    p_re = 0;
  }
  if (p_report < 0) {
    errmsg = e_positive;
    p_report = 1;
  }
  if ((p_sj < -100 || p_sj >= Rows) && full_screen) {
    if (Rows != old_Rows)       /* Rows changed, just adjust p_sj */
      p_sj = Rows / 2;
    else {
      errmsg = e_scroll;
      p_sj = 1;
    }
  }
  if (p_so < 0 && full_screen) {
    errmsg = e_scroll;
    p_so = 0;
  }
  if (p_siso < 0 && full_screen) {
    errmsg = e_positive;
    p_siso = 0;
  }
  if (p_cwh < 1) {
    errmsg = e_positive;
    p_cwh = 1;
  }
  if (p_ut < 0) {
    errmsg = e_positive;
    p_ut = 2000;
  }
  if (p_ss < 0) {
    errmsg = e_positive;
    p_ss = 0;
  }

  /* May set global value for local option. */
  if ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = *pp;

  options[opt_idx].flags |= P_WAS_SET;

  if (!starting && errmsg == NULL) {
    char buf_old[NUMBUFLEN];
    char buf_new[NUMBUFLEN];
    char buf_type[7];
    vim_snprintf(buf_old, ARRAY_SIZE(buf_old), "%ld", old_value);
    vim_snprintf(buf_new, ARRAY_SIZE(buf_new), "%ld", value);
    vim_snprintf(buf_type, ARRAY_SIZE(buf_type), "%s",
                 (opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_NEW, buf_new, -1);
    set_vim_var_string(VV_OPTION_OLD, buf_old, -1);
    set_vim_var_string(VV_OPTION_TYPE, buf_type, -1);
    apply_autocmds(EVENT_OPTIONSET,
                   (char_u *) options[opt_idx].fullname,
                   NULL, false, NULL);
    reset_v_option_vars();
  }

  comp_col();                       /* in case 'columns' or 'ls' changed */
  if (curwin->w_curswant != MAXCOL
      && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0)
    curwin->w_set_curswant = TRUE;
  check_redraw(options[opt_idx].flags);

  return errmsg;
}