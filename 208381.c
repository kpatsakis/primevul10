void set_init_1(void)
{
  int opt_idx;

  langmap_init();

  /* Be nocompatible */
  p_cp = FALSE;

  /*
   * Find default value for 'shell' option.
   * Don't use it if it is empty.
   */
  {
    const char *shell = os_getenv("SHELL");
    if (shell != NULL) {
      set_string_default("sh", (char *) shell, false);
    }
  }

  /*
   * Set the default for 'backupskip' to include environment variables for
   * temp files.
   */
  {
# ifdef UNIX
    static char     *(names[4]) = {"", "TMPDIR", "TEMP", "TMP"};
# else
    static char     *(names[3]) = {"TMPDIR", "TEMP", "TMP"};
# endif
    int len;
    garray_T ga;

    ga_init(&ga, 1, 100);
    for (size_t n = 0; n < ARRAY_SIZE(names); ++n) {
      bool mustfree = true;
      char *p;
# ifdef UNIX
      if (*names[n] == NUL) {
        p = "/tmp";
        mustfree = false;
      }
      else
# endif
      p = vim_getenv(names[n]);
      if (p != NULL && *p != NUL) {
        // First time count the NUL, otherwise count the ','.
        len = (int)strlen(p) + 3;
        ga_grow(&ga, len);
        if (!GA_EMPTY(&ga))
          STRCAT(ga.ga_data, ",");
        STRCAT(ga.ga_data, p);
        add_pathsep(ga.ga_data);
        STRCAT(ga.ga_data, "*");
        ga.ga_len += len;
      }
      if(mustfree) {
        xfree(p);
      }
    }
    if (ga.ga_data != NULL) {
      set_string_default("bsk", ga.ga_data, true);
    }
  }

  /*
   * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory
   */
  opt_idx = findoption((char_u *)"maxmemtot");
  if (opt_idx >= 0) {
    {
      /* Use half of amount of memory available to Vim. */
      /* If too much to fit in uintptr_t, get uintptr_t max */
      uint64_t available_kib = os_get_total_mem_kib();
      uintptr_t n = available_kib / 2 > UINTPTR_MAX
                    ? UINTPTR_MAX
                    : (uintptr_t)(available_kib /2);
      options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
      opt_idx = findoption((char_u *)"maxmem");
      if (opt_idx >= 0) {
        options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
      }
    }
  }


  {
    char_u  *cdpath;
    char_u  *buf;
    int i;
    int j;

    /* Initialize the 'cdpath' option's default value. */
    cdpath = (char_u *)vim_getenv("CDPATH");
    if (cdpath != NULL) {
      buf = xmalloc(2 * STRLEN(cdpath) + 2);
      {
        buf[0] = ',';               /* start with ",", current dir first */
        j = 1;
        for (i = 0; cdpath[i] != NUL; ++i) {
          if (vim_ispathlistsep(cdpath[i]))
            buf[j++] = ',';
          else {
            if (cdpath[i] == ' ' || cdpath[i] == ',')
              buf[j++] = '\\';
            buf[j++] = cdpath[i];
          }
        }
        buf[j] = NUL;
        opt_idx = findoption((char_u *)"cdpath");
        if (opt_idx >= 0) {
          options[opt_idx].def_val[VI_DEFAULT] = buf;
          options[opt_idx].flags |= P_DEF_ALLOCED;
        } else
          xfree(buf);           /* cannot happen */
      }
      xfree(cdpath);
    }
  }

#if defined(MSWIN) || defined(MAC)
  /* Set print encoding on platforms that don't default to latin1 */
  set_string_default("printencoding", "hp-roman8", false);
#endif

  // 'printexpr' must be allocated to be able to evaluate it.
  set_string_default("printexpr",
#ifdef UNIX
                     "system(['lpr'] "
                            "+ (empty(&printdevice)?[]:['-P', &printdevice]) "
                            "+ [v:fname_in])"
                     ". delete(v:fname_in)"
                     "+ v:shell_error",
#elif defined(MSWIN)
                     "system(['copy', v:fname_in, "
                             "empty(&printdevice)?'LPT1':&printdevice])"
                     ". delete(v:fname_in)",
#else
                     "",
#endif
                     false);

  char *backupdir = stdpaths_user_data_subpath("backup", 0, true);
  const size_t backupdir_len = strlen(backupdir);
  backupdir = xrealloc(backupdir, backupdir_len + 3);
  memmove(backupdir + 2, backupdir, backupdir_len + 1);
  memmove(backupdir, ".,", 2);
  set_string_default("viewdir", stdpaths_user_data_subpath("view", 0, true),
                     true);
  set_string_default("backupdir", backupdir, true);
  set_string_default("directory", stdpaths_user_data_subpath("swap", 2, true),
                     true);
  set_string_default("undodir", stdpaths_user_data_subpath("undo", 0, true),
                     true);
  // Set default for &runtimepath. All necessary expansions are performed in
  // this function.
  set_runtimepath_default();

  /*
   * Set all the options (except the terminal options) to their default
   * value.  Also set the global value for local options.
   */
  set_options_default(0);


  curbuf->b_p_initialized = true;
  curbuf->b_p_ar = -1;          /* no local 'autoread' value */
  curbuf->b_p_ul = NO_LOCAL_UNDOLEVEL;
  check_buf_options(curbuf);
  check_win_options(curwin);
  check_options();

  /* Set all options to their Vim default */
  set_options_default(OPT_FREE);

  // set 'laststatus'
  last_status(false);

  /* Must be before option_expand(), because that one needs vim_isIDc() */
  didset_options();

  // Use the current chartab for the generic chartab. This is not in
  // didset_options() because it only depends on 'encoding'.
  init_spell_chartab();

  /*
   * Expand environment variables and things like "~" for the defaults.
   * If option_expand() returns non-NULL the variable is expanded.  This can
   * only happen for non-indirect options.
   * Also set the default to the expanded value, so ":set" does not list
   * them.
   * Don't set the P_ALLOCED flag, because we don't want to free the
   * default.
   */
  for (opt_idx = 0; options[opt_idx].fullname; opt_idx++) {
    if (options[opt_idx].flags & P_NO_DEF_EXP) {
      continue;
    }
    char *p;
    if ((options[opt_idx].flags & P_GETTEXT)
        && options[opt_idx].var != NULL) {
      p = _(*(char **)options[opt_idx].var);
    } else {
      p = (char *) option_expand(opt_idx, NULL);
    }
    if (p != NULL) {
      p = xstrdup(p);
      *(char **)options[opt_idx].var = p;
      /* VIMEXP
       * Defaults for all expanded options are currently the same for Vi
       * and Vim.  When this changes, add some code here!  Also need to
       * split P_DEF_ALLOCED in two.
       */
      if (options[opt_idx].flags & P_DEF_ALLOCED)
        xfree(options[opt_idx].def_val[VI_DEFAULT]);
      options[opt_idx].def_val[VI_DEFAULT] = (char_u *) p;
      options[opt_idx].flags |= P_DEF_ALLOCED;
    }
  }

  save_file_ff(curbuf);         /* Buffer is unchanged */

  /* Detect use of mlterm.
   * Mlterm is a terminal emulator akin to xterm that has some special
   * abilities (bidi namely).
   * NOTE: mlterm's author is being asked to 'set' a variable
   *       instead of an environment variable due to inheritance.
   */
  if (os_env_exists("MLTERM"))
    set_option_value((char_u *)"tbidi", 1L, NULL, 0);

  didset_options2();

  // enc_locale() will try to find the encoding of the current locale.
  // This will be used when 'default' is used as encoding specifier
  // in 'fileencodings'
  char_u *p = enc_locale();
  if (p == NULL) {
      // use utf-8 as 'default' if locale encoding can't be detected.
      p = vim_strsave((char_u *)"utf-8");
  }
  fenc_default = p;

#ifdef HAVE_WORKING_LIBINTL
  // GNU gettext 0.10.37 supports this feature: set the codeset used for
  // translated messages independently from the current locale.
  (void)bind_textdomain_codeset(PROJECT_NAME, (char *)p_enc);
#endif

  /* Set the default for 'helplang'. */
  set_helplang_default(get_mess_lang());
}