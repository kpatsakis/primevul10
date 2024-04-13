set_bool_option (
    int opt_idx,                            /* index in options[] table */
    char_u *varp,                      /* pointer to the option variable */
    int value,                              /* new value */
    int opt_flags                          /* OPT_LOCAL and/or OPT_GLOBAL */
)
{
  int old_value = *(int *)varp;

  /* Disallow changing some options from secure mode */
  if ((secure || sandbox != 0)
      && (options[opt_idx].flags & P_SECURE)) {
    return e_secure;
  }

  *(int *)varp = value;             /* set the new value */
  /* Remember where the option was set. */
  set_option_scriptID_idx(opt_idx, opt_flags, current_SID);


  /* May set global value for local option. */
  if ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    *(int *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = value;

  // Ensure that options set to p_force_on cannot be disabled.
  if ((int *)varp == &p_force_on && p_force_on == FALSE) {
    p_force_on = TRUE;
    return e_unsupportedoption;
  }
  // Ensure that options set to p_force_off cannot be enabled.
  else if ((int *)varp == &p_force_off && p_force_off == TRUE) {
    p_force_off = FALSE;
    return e_unsupportedoption;
  }
  /* 'undofile' */
  else if ((int *)varp == &curbuf->b_p_udf || (int *)varp == &p_udf) {
    /* Only take action when the option was set. When reset we do not
     * delete the undo file, the option may be set again without making
     * any changes in between. */
    if (curbuf->b_p_udf || p_udf) {
      char_u hash[UNDO_HASH_SIZE];
      buf_T       *save_curbuf = curbuf;

      for (curbuf = firstbuf; curbuf != NULL; curbuf = curbuf->b_next) {
        /* When 'undofile' is set globally: for every buffer, otherwise
         * only for the current buffer: Try to read in the undofile,
         * if one exists, the buffer wasn't changed and the buffer was
         * loaded */
        if ((curbuf == save_curbuf
             || (opt_flags & OPT_GLOBAL) || opt_flags == 0)
            && !curbufIsChanged() && curbuf->b_ml.ml_mfp != NULL) {
          u_compute_hash(hash);
          u_read_undo(NULL, hash, curbuf->b_fname);
        }
      }
      curbuf = save_curbuf;
    }
  } else if ((int *)varp == &curbuf->b_p_ro) {
    /* when 'readonly' is reset globally, also reset readonlymode */
    if (!curbuf->b_p_ro && (opt_flags & OPT_LOCAL) == 0)
      readonlymode = FALSE;

    /* when 'readonly' is set may give W10 again */
    if (curbuf->b_p_ro)
      curbuf->b_did_warn = false;

    redraw_titles();
  }
  /* when 'modifiable' is changed, redraw the window title */
  else if ((int *)varp == &curbuf->b_p_ma) {
    redraw_titles();
  }
  /* when 'endofline' is changed, redraw the window title */
  else if ((int *)varp == &curbuf->b_p_eol) {
    redraw_titles();
  } else if ((int *)varp == &curbuf->b_p_fixeol) {
    // when 'fixeol' is changed, redraw the window title
    redraw_titles();
  }
  /* when 'bomb' is changed, redraw the window title and tab page text */
  else if ((int *)varp == &curbuf->b_p_bomb) {
    redraw_titles();
  }
  /* when 'bin' is set also set some other options */
  else if ((int *)varp == &curbuf->b_p_bin) {
    set_options_bin(old_value, curbuf->b_p_bin, opt_flags);
    redraw_titles();
  }
  /* when 'buflisted' changes, trigger autocommands */
  else if ((int *)varp == &curbuf->b_p_bl && old_value != curbuf->b_p_bl) {
    apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
        NULL, NULL, TRUE, curbuf);
  }
  /* when 'swf' is set, create swapfile, when reset remove swapfile */
  else if ((int *)varp == (int *)&curbuf->b_p_swf) {
    if (curbuf->b_p_swf && p_uc)
      ml_open_file(curbuf);                     /* create the swap file */
    else
      /* no need to reset curbuf->b_may_swap, ml_open_file() will check
       * buf->b_p_swf */
      mf_close_file(curbuf, true);              /* remove the swap file */
  }
  /* when 'terse' is set change 'shortmess' */
  else if ((int *)varp == &p_terse) {
    char_u  *p;

    p = vim_strchr(p_shm, SHM_SEARCH);

    /* insert 's' in p_shm */
    if (p_terse && p == NULL) {
      STRCPY(IObuff, p_shm);
      STRCAT(IObuff, "s");
      set_string_option_direct((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    /* remove 's' from p_shm */
    else if (!p_terse && p != NULL)
      STRMOVE(p, p + 1);
  }
  /* when 'paste' is set or reset also change other options */
  else if ((int *)varp == &p_paste) {
    paste_option_changed();
  }
  /* when 'insertmode' is set from an autocommand need to do work here */
  else if ((int *)varp == &p_im) {
    if (p_im) {
      if ((State & INSERT) == 0) {
        need_start_insertmode = true;
      }
      stop_insert_mode = false;
    } else if (old_value) {  // only reset if it was set previously
      need_start_insertmode = false;
      stop_insert_mode = true;
      if (restart_edit != 0 && mode_displayed) {
        clear_cmdline = true;  // remove "(insert)"
      }
      restart_edit = 0;
    }
  }
  /* when 'ignorecase' is set or reset and 'hlsearch' is set, redraw */
  else if ((int *)varp == &p_ic && p_hls) {
    redraw_all_later(SOME_VALID);
  }
  /* when 'hlsearch' is set or reset: reset no_hlsearch */
  else if ((int *)varp == &p_hls) {
    SET_NO_HLSEARCH(FALSE);
  }
  /* when 'scrollbind' is set: snapshot the current position to avoid a jump
   * at the end of normal_cmd() */
  else if ((int *)varp == &curwin->w_p_scb) {
    if (curwin->w_p_scb) {
      do_check_scrollbind(FALSE);
      curwin->w_scbind_pos = curwin->w_topline;
    }
  }
  /* There can be only one window with 'previewwindow' set. */
  else if ((int *)varp == &curwin->w_p_pvw) {
    if (curwin->w_p_pvw) {
      FOR_ALL_WINDOWS_IN_TAB(win, curtab) {
        if (win->w_p_pvw && win != curwin) {
          curwin->w_p_pvw = FALSE;
          return (char_u *)N_("E590: A preview window already exists");
        }
      }
    }
  } else if (varp == (char_u *)&(curbuf->b_p_lisp)) {
    // When 'lisp' option changes include/exclude '-' in
    // keyword characters.
    (void)buf_init_chartab(curbuf, false);          // ignore errors
  } else if ((int *)varp == &p_title) {
    // when 'title' changed, may need to change the title; same for 'icon'
    did_set_title(false);
  } else if ((int *)varp == &p_icon) {
    did_set_title(true);
  } else if ((int *)varp == &curbuf->b_changed) {
    if (!value) {
      save_file_ff(curbuf);             // Buffer is unchanged
    }
    redraw_titles();
    modified_was_set = value;
  }

#ifdef BACKSLASH_IN_FILENAME
  else if ((int *)varp == &p_ssl) {
    if (p_ssl) {
      psepc = '/';
      psepcN = '\\';
      pseps[0] = '/';
    } else {
      psepc = '\\';
      psepcN = '/';
      pseps[0] = '\\';
    }

    /* need to adjust the file name arguments and buffer names. */
    buflist_slash_adjust();
    alist_slash_adjust();
    scriptnames_slash_adjust();
  }
#endif

  /* If 'wrap' is set, set w_leftcol to zero. */
  else if ((int *)varp == &curwin->w_p_wrap) {
    if (curwin->w_p_wrap)
      curwin->w_leftcol = 0;
  } else if ((int *)varp == &p_ea) {
    if (p_ea && !old_value) {
      win_equal(curwin, false, 0);
    }
  } else if ((int *)varp == &p_acd) {
    // Change directories when the 'acd' option is set now.
    do_autochdir();
  }
  /* 'diff' */
  else if ((int *)varp == &curwin->w_p_diff) {
    /* May add or remove the buffer from the list of diff buffers. */
    diff_buf_adjust(curwin);
    if (foldmethodIsDiff(curwin))
      foldUpdateAll(curwin);
  }


  /* 'spell' */
  else if ((int *)varp == &curwin->w_p_spell) {
    if (curwin->w_p_spell) {
      char_u      *errmsg = did_set_spelllang(curwin);
      if (errmsg != NULL)
        EMSG(_(errmsg));
    }
  } else if ((int *)varp == &p_altkeymap) {
    if (old_value != p_altkeymap) {
      if (!p_altkeymap) {
        p_hkmap = p_fkmap;
        p_fkmap = 0;
      } else {
        p_fkmap = p_hkmap;
        p_hkmap = 0;
      }
      (void)init_chartab();
    }
  }

  /*
   * In case some second language keymapping options have changed, check
   * and correct the setting in a consistent way.
   */

  /*
   * If hkmap or fkmap are set, reset Arabic keymapping.
   */
  if ((p_hkmap || p_fkmap) && p_altkeymap) {
    p_altkeymap = p_fkmap;
    curwin->w_p_arab = FALSE;
    (void)init_chartab();
  }

  /*
   * If hkmap set, reset Farsi keymapping.
   */
  if (p_hkmap && p_altkeymap) {
    p_altkeymap = 0;
    p_fkmap = 0;
    curwin->w_p_arab = FALSE;
    (void)init_chartab();
  }

  /*
   * If fkmap set, reset Hebrew keymapping.
   */
  if (p_fkmap && !p_altkeymap) {
    p_altkeymap = 1;
    p_hkmap = 0;
    curwin->w_p_arab = FALSE;
    (void)init_chartab();
  }

  if ((int *)varp == &curwin->w_p_arab) {
    if (curwin->w_p_arab) {
      /*
       * 'arabic' is set, handle various sub-settings.
       */
      if (!p_tbidi) {
        /* set rightleft mode */
        if (!curwin->w_p_rl) {
          curwin->w_p_rl = TRUE;
          changed_window_setting();
        }

        /* Enable Arabic shaping (major part of what Arabic requires) */
        if (!p_arshape) {
          p_arshape = TRUE;
          redraw_later_clear();
        }
      }

      /* Arabic requires a utf-8 encoding, inform the user if its not
       * set. */
      if (STRCMP(p_enc, "utf-8") != 0) {
        static char *w_arabic = N_(
            "W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

        msg_source(hl_attr(HLF_W));
        MSG_ATTR(_(w_arabic), hl_attr(HLF_W));
        set_vim_var_string(VV_WARNINGMSG, _(w_arabic), -1);
      }

      /* set 'delcombine' */
      p_deco = TRUE;

      /* Force-set the necessary keymap for arabic */
      set_option_value((char_u *)"keymap", 0L, (char_u *)"arabic",
          OPT_LOCAL);
      p_altkeymap = 0;
      p_hkmap = 0;
      p_fkmap = 0;
      (void)init_chartab();
    } else {
      /*
       * 'arabic' is reset, handle various sub-settings.
       */
      if (!p_tbidi) {
        /* reset rightleft mode */
        if (curwin->w_p_rl) {
          curwin->w_p_rl = FALSE;
          changed_window_setting();
        }

        /* 'arabicshape' isn't reset, it is a global option and
         * another window may still need it "on". */
      }

      /* 'delcombine' isn't reset, it is a global option and another
       * window may still want it "on". */

      /* Revert to the default keymap */
      curbuf->b_p_iminsert = B_IMODE_NONE;
      curbuf->b_p_imsearch = B_IMODE_USE_INSERT;
    }
  }


  /*
   * End of handling side effects for bool options.
   */

  // after handling side effects, call autocommand

  options[opt_idx].flags |= P_WAS_SET;

  if (!starting) {
    char buf_old[2];
    char buf_new[2];
    char buf_type[7];
    vim_snprintf(buf_old, ARRAY_SIZE(buf_old), "%d",
                 old_value ? true: false);
    vim_snprintf(buf_new, ARRAY_SIZE(buf_new), "%d",
                 value ? true: false);
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

  comp_col();                       /* in case 'ruler' or 'showcmd' changed */
  if (curwin->w_curswant != MAXCOL
      && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0)
    curwin->w_set_curswant = TRUE;
  check_redraw(options[opt_idx].flags);

  return NULL;
}