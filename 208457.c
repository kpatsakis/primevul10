do_set (
    char_u *arg,               /* option string (may be written to!) */
    int opt_flags
)
{
  int opt_idx;
  char_u      *errmsg;
  char_u errbuf[80];
  char_u      *startarg;
  int prefix;           /* 1: nothing, 0: "no", 2: "inv" in front of name */
  char_u nextchar;                  /* next non-white char after option name */
  int afterchar;                    /* character just after option name */
  int len;
  int i;
  long value;
  int key;
  uint32_t flags;                   /* flags for current option */
  char_u      *varp = NULL;         /* pointer to variable for current option */
  int did_show = FALSE;             /* already showed one value */
  int adding;                       /* "opt+=arg" */
  int prepending;                   /* "opt^=arg" */
  int removing;                     /* "opt-=arg" */
  int cp_val = 0;

  if (*arg == NUL) {
    showoptions(0, opt_flags);
    did_show = TRUE;
    goto theend;
  }

  while (*arg != NUL) {         /* loop to process all options */
    errmsg = NULL;
    startarg = arg;             /* remember for error message */

    if (STRNCMP(arg, "all", 3) == 0 && !isalpha(arg[3])
        && !(opt_flags & OPT_MODELINE)) {
      /*
       * ":set all"  show all options.
       * ":set all&" set all options to their default value.
       */
      arg += 3;
      if (*arg == '&') {
        arg++;
        // Only for :set command set global value of local options.
        set_options_default(OPT_FREE | opt_flags);
        didset_options();
        didset_options2();
        redraw_all_later(CLEAR);
      } else {
        showoptions(1, opt_flags);
        did_show = TRUE;
      }
    } else if (STRNCMP(arg, "termcap",
                   7) == 0 && !(opt_flags & OPT_MODELINE)) {
      did_show = TRUE;
      arg += 7;
    } else {
      prefix = 1;
      if (STRNCMP(arg, "no", 2) == 0) {
        prefix = 0;
        arg += 2;
      } else if (STRNCMP(arg, "inv", 3) == 0) {
        prefix = 2;
        arg += 3;
      }

      /* find end of name */
      key = 0;
      if (*arg == '<') {
        opt_idx = -1;
        /* look out for <t_>;> */
        if (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
          len = 5;
        else {
          len = 1;
          while (arg[len] != NUL && arg[len] != '>')
            ++len;
        }
        if (arg[len] != '>') {
          errmsg = e_invarg;
          goto skip;
        }
        if (arg[1] == 't' && arg[2] == '_') {  // could be term code
          opt_idx = findoption_len(arg + 1, (size_t) (len - 1));
        }
        len++;
        if (opt_idx == -1) {
          key = find_key_option(arg + 1);
        }
      } else {
        len = 0;
        // The two characters after "t_" may not be alphanumeric.
        if (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3]) {
          len = 4;
        } else {
          while (ASCII_ISALNUM(arg[len]) || arg[len] == '_') {
            len++;
          }
        }
        opt_idx = findoption_len(arg, (size_t) len);
        if (opt_idx == -1) {
          key = find_key_option(arg);
        }
      }

      /* remember character after option name */
      afterchar = arg[len];

      /* skip white space, allow ":set ai  ?" */
      while (ascii_iswhite(arg[len]))
        ++len;

      adding = FALSE;
      prepending = FALSE;
      removing = FALSE;
      if (arg[len] != NUL && arg[len + 1] == '=') {
        if (arg[len] == '+') {
          adding = TRUE;                        /* "+=" */
          ++len;
        } else if (arg[len] == '^') {
          prepending = TRUE;                    /* "^=" */
          ++len;
        } else if (arg[len] == '-') {
          removing = TRUE;                      /* "-=" */
          ++len;
        }
      }
      nextchar = arg[len];

      if (opt_idx == -1 && key == 0) {          /* found a mismatch: skip */
        errmsg = (char_u *)N_("E518: Unknown option");
        goto skip;
      }

      if (opt_idx >= 0) {
        if (options[opt_idx].var == NULL) {         /* hidden option: skip */
          /* Only give an error message when requesting the value of
           * a hidden option, ignore setting it. */
          if (vim_strchr((char_u *)"=:!&<", nextchar) == NULL
              && (!(options[opt_idx].flags & P_BOOL)
                  || nextchar == '?'))
            errmsg = (char_u *)_(e_unsupportedoption);
          goto skip;
        }

        flags = options[opt_idx].flags;
        varp = get_varp_scope(&(options[opt_idx]), opt_flags);
      } else {
        flags = P_STRING;
      }

      /* Skip all options that are not window-local (used when showing
       * an already loaded buffer in a window). */
      if ((opt_flags & OPT_WINONLY)
          && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
        goto skip;

      /* Skip all options that are window-local (used for :vimgrep). */
      if ((opt_flags & OPT_NOWIN) && opt_idx >= 0
          && options[opt_idx].var == VAR_WIN)
        goto skip;

      /* Disallow changing some options from modelines. */
      if (opt_flags & OPT_MODELINE) {
        if (flags & (P_SECURE | P_NO_ML)) {
          errmsg = (char_u *)_("E520: Not allowed in a modeline");
          goto skip;
        }
        /* In diff mode some options are overruled.  This avoids that
         * 'foldmethod' becomes "marker" instead of "diff" and that
         * "wrap" gets set. */
        if (curwin->w_p_diff
            && opt_idx >= 0              /* shut up coverity warning */
            && (options[opt_idx].indir == PV_FDM
                || options[opt_idx].indir == PV_WRAP))
          goto skip;
      }

      /* Disallow changing some options in the sandbox */
      if (sandbox != 0 && (flags & P_SECURE)) {
        errmsg = (char_u *)_(e_sandbox);
        goto skip;
      }

      if (vim_strchr((char_u *)"?=:!&<", nextchar) != NULL) {
        arg += len;
        cp_val = p_cp;
        if (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i') {
          if (arg[3] == 'm') {          /* "opt&vim": set to Vim default */
            cp_val = FALSE;
            arg += 3;
          } else {                    /* "opt&vi": set to Vi default */
            cp_val = TRUE;
            arg += 2;
          }
        }
        if (vim_strchr((char_u *)"?!&<", nextchar) != NULL
            && arg[1] != NUL && !ascii_iswhite(arg[1])) {
          errmsg = e_trailing;
          goto skip;
        }
      }

      /*
       * allow '=' and ':' as MSDOS command.com allows only one
       * '=' character per "set" command line. grrr. (jw)
       */
      if (nextchar == '?'
          || (prefix == 1
              && vim_strchr((char_u *)"=:&<", nextchar) == NULL
              && !(flags & P_BOOL))) {
        /*
         * print value
         */
        if (did_show)
          msg_putchar('\n');                /* cursor below last one */
        else {
          gotocmdline(TRUE);                /* cursor at status line */
          did_show = TRUE;                  /* remember that we did a line */
        }
        if (opt_idx >= 0) {
          showoneopt(&options[opt_idx], opt_flags);
          if (p_verbose > 0) {
            /* Mention where the option was last set. */
            if (varp == options[opt_idx].var)
              last_set_msg(options[opt_idx].scriptID);
            else if ((int)options[opt_idx].indir & PV_WIN)
              last_set_msg(curwin->w_p_scriptID[
                    (int)options[opt_idx].indir & PV_MASK]);
            else if ((int)options[opt_idx].indir & PV_BUF)
              last_set_msg(curbuf->b_p_scriptID[
                    (int)options[opt_idx].indir & PV_MASK]);
          }
        } else {
          errmsg = (char_u *)N_("E846: Key code not set");
          goto skip;
        }
        if (nextchar != '?'
            && nextchar != NUL && !ascii_iswhite(afterchar))
          errmsg = e_trailing;
      } else {
        if (flags & P_BOOL) {                       /* boolean */
          if (nextchar == '=' || nextchar == ':') {
            errmsg = e_invarg;
            goto skip;
          }

          /*
           * ":set opt!": invert
           * ":set opt&": reset to default value
           * ":set opt<": reset to global value
           */
          if (nextchar == '!')
            value = *(int *)(varp) ^ 1;
          else if (nextchar == '&')
            value = (int)(intptr_t)options[opt_idx].def_val[
              ((flags & P_VI_DEF) || cp_val)
              ?  VI_DEFAULT : VIM_DEFAULT];
          else if (nextchar == '<') {
            /* For 'autoread' -1 means to use global value. */
            if ((int *)varp == &curbuf->b_p_ar
                && opt_flags == OPT_LOCAL)
              value = -1;
            else
              value = *(int *)get_varp_scope(&(options[opt_idx]),
                  OPT_GLOBAL);
          } else {
            /*
             * ":set invopt": invert
             * ":set opt" or ":set noopt": set or reset
             */
            if (nextchar != NUL && !ascii_iswhite(afterchar)) {
              errmsg = e_trailing;
              goto skip;
            }
            if (prefix == 2)                    /* inv */
              value = *(int *)(varp) ^ 1;
            else
              value = prefix;
          }

          errmsg = set_bool_option(opt_idx, varp, (int)value,
              opt_flags);
        } else {                                  /* numeric or string */
          if (vim_strchr((char_u *)"=:&<", nextchar) == NULL
              || prefix != 1) {
            errmsg = e_invarg;
            goto skip;
          }

          if (flags & P_NUM) {                      /* numeric */
            /*
             * Different ways to set a number option:
             * &	    set to default value
             * <	    set to global value
             * <xx>	    accept special key codes for 'wildchar'
             * c	    accept any non-digit for 'wildchar'
             * [-]0-9   set number
             * other    error
             */
            ++arg;
            if (nextchar == '&')
              value = (long)options[opt_idx].def_val[
                ((flags & P_VI_DEF) || cp_val)
                ?  VI_DEFAULT : VIM_DEFAULT];
            else if (nextchar == '<') {
              /* For 'undolevels' NO_LOCAL_UNDOLEVEL means to
               * use the global value. */
              if ((long *)varp == &curbuf->b_p_ul
                  && opt_flags == OPT_LOCAL)
                value = NO_LOCAL_UNDOLEVEL;
              else
                value = *(long *)get_varp_scope(
                    &(options[opt_idx]), OPT_GLOBAL);
            } else if (((long *)varp == &p_wc
                        || (long *)varp == &p_wcm)
                       && (*arg == '<'
                           || *arg == '^'
                           || ((!arg[1] || ascii_iswhite(arg[1]))
                               && !ascii_isdigit(*arg)))) {
              value = string_to_key(arg);
              if (value == 0 && (long *)varp != &p_wcm) {
                errmsg = e_invarg;
                goto skip;
              }
            } else if (*arg == '-' || ascii_isdigit(*arg)) {
              // Allow negative (for 'undolevels'), octal and
              // hex numbers.
              vim_str2nr(arg, NULL, &i, STR2NR_ALL, &value, NULL, 0);
              if (arg[i] != NUL && !ascii_iswhite(arg[i])) {
                errmsg = e_invarg;
                goto skip;
              }
            } else {
              errmsg = (char_u *)N_("E521: Number required after =");
              goto skip;
            }

            if (adding)
              value = *(long *)varp + value;
            if (prepending)
              value = *(long *)varp * value;
            if (removing)
              value = *(long *)varp - value;
            errmsg = set_num_option(opt_idx, varp, value,
                errbuf, sizeof(errbuf), opt_flags);
          } else if (opt_idx >= 0) {                      /* string */
            char_u      *save_arg = NULL;
            char_u      *s = NULL;
            char_u      *oldval = NULL;         // previous value if *varp
            char_u      *newval;
            char_u      *origval = NULL;
            char *saved_origval = NULL;
            unsigned newlen;
            int comma;
            int bs;
            int new_value_alloced;                      /* new string option
                                                           was allocated */

            /* When using ":set opt=val" for a global option
             * with a local value the local value will be
             * reset, use the global value here. */
            if ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
                && ((int)options[opt_idx].indir & PV_BOTH))
              varp = options[opt_idx].var;

            /* The old value is kept until we are sure that the
             * new value is valid. */
            oldval = *(char_u **)varp;
            if (nextchar == '&') {              /* set to default val */
              newval = options[opt_idx].def_val[
                ((flags & P_VI_DEF) || cp_val)
                ?  VI_DEFAULT : VIM_DEFAULT];
              /* expand environment variables and ~ (since the
               * default value was already expanded, only
               * required when an environment variable was set
               * later */
              new_value_alloced = true;
              if (newval == NULL) {
                newval = empty_option;
              } else if (!(options[opt_idx].flags | P_NO_DEF_EXP)) {
                s = option_expand(opt_idx, newval);
                if (s == NULL) {
                  s = newval;
                }
                newval = vim_strsave(s);
              } else {
                newval = (char_u *)xstrdup((char *)newval);
              }
            } else if (nextchar == '<') {  // set to global val
              newval = vim_strsave(*(char_u **)get_varp_scope(
                      &(options[opt_idx]), OPT_GLOBAL));
              new_value_alloced = TRUE;
            } else {
              ++arg;                    /* jump to after the '=' or ':' */

              /*
               * Set 'keywordprg' to ":help" if an empty
               * value was passed to :set by the user.
               * Misuse errbuf[] for the resulting string.
               */
              if (varp == (char_u *)&p_kp
                  && (*arg == NUL || *arg == ' ')) {
                STRCPY(errbuf, ":help");
                save_arg = arg;
                arg = errbuf;
              }
              /*
               * Convert 'backspace' number to string, for
               * adding, prepending and removing string.
               */
              else if (varp == (char_u *)&p_bs
                       && ascii_isdigit(**(char_u **)varp)) {
                i = getdigits_int((char_u **)varp);
                switch (i) {
                case 0:
                  *(char_u **)varp = empty_option;
                  break;
                case 1:
                  *(char_u **)varp = vim_strsave(
                      (char_u *)"indent,eol");
                  break;
                case 2:
                  *(char_u **)varp = vim_strsave(
                      (char_u *)"indent,eol,start");
                  break;
                }
                xfree(oldval);
                oldval = *(char_u **)varp;
              }
              /*
               * Convert 'whichwrap' number to string, for
               * backwards compatibility with Vim 3.0.
               * Misuse errbuf[] for the resulting string.
               */
              else if (varp == (char_u *)&p_ww
                       && ascii_isdigit(*arg)) {
                *errbuf = NUL;
                i = getdigits_int(&arg);
                if (i & 1)
                  STRCAT(errbuf, "b,");
                if (i & 2)
                  STRCAT(errbuf, "s,");
                if (i & 4)
                  STRCAT(errbuf, "h,l,");
                if (i & 8)
                  STRCAT(errbuf, "<,>,");
                if (i & 16)
                  STRCAT(errbuf, "[,],");
                if (*errbuf != NUL)                     /* remove trailing , */
                  errbuf[STRLEN(errbuf) - 1] = NUL;
                save_arg = arg;
                arg = errbuf;
              }
              /*
               * Remove '>' before 'dir' and 'bdir', for
               * backwards compatibility with version 3.0
               */
              else if (  *arg == '>'
                         && (varp == (char_u *)&p_dir
                             || varp == (char_u *)&p_bdir)) {
                ++arg;
              }

              /* When setting the local value of a global
               * option, the old value may be the global value. */
              if (((int)options[opt_idx].indir & PV_BOTH)
                  && (opt_flags & OPT_LOCAL))
                origval = *(char_u **)get_varp(
                    &options[opt_idx]);
              else
                origval = oldval;

              /*
               * Copy the new string into allocated memory.
               * Can't use set_string_option_direct(), because
               * we need to remove the backslashes.
               */
              /* get a bit too much */
              newlen = (unsigned)STRLEN(arg) + 1;
              if (adding || prepending || removing)
                newlen += (unsigned)STRLEN(origval) + 1;
              newval = xmalloc(newlen);
              s = newval;

              /*
               * Copy the string, skip over escaped chars.
               * For WIN32 backslashes before normal
               * file name characters are not removed, and keep
               * backslash at start, for "\\machine\path", but
               * do remove it for "\\\\machine\\path".
               * The reverse is found in ExpandOldSetting().
               */
              while (*arg && !ascii_iswhite(*arg)) {
                if (*arg == '\\' && arg[1] != NUL
#ifdef BACKSLASH_IN_FILENAME
                    && !((flags & P_EXPAND)
                         && vim_isfilec(arg[1])
                         && (arg[1] != '\\'
                             || (s == newval
                                 && arg[2] != '\\')))
#endif
                    )
                  ++arg;                        /* remove backslash */
                if (has_mbyte
                    && (i = (*mb_ptr2len)(arg)) > 1) {
                  /* copy multibyte char */
                  memmove(s, arg, (size_t)i);
                  arg += i;
                  s += i;
                } else
                  *s++ = *arg++;
              }
              *s = NUL;

              /*
               * Expand environment variables and ~.
               * Don't do it when adding without inserting a
               * comma.
               */
              if (!(adding || prepending || removing)
                  || (flags & P_COMMA)) {
                s = option_expand(opt_idx, newval);
                if (s != NULL) {
                  xfree(newval);
                  newlen = (unsigned)STRLEN(s) + 1;
                  if (adding || prepending || removing)
                    newlen += (unsigned)STRLEN(origval) + 1;
                  newval = xmalloc(newlen);
                  STRCPY(newval, s);
                }
              }

              /* locate newval[] in origval[] when removing it
               * and when adding to avoid duplicates */
              i = 0;                    /* init for GCC */
              if (removing || (flags & P_NODUP)) {
                i = (int)STRLEN(newval);
                bs = 0;
                for (s = origval; *s; ++s) {
                  if ((!(flags & P_COMMA)
                       || s == origval
                       || (s[-1] == ',' && !(bs & 1)))
                      && STRNCMP(s, newval, i) == 0
                      && (!(flags & P_COMMA)
                          || s[i] == ','
                          || s[i] == NUL)) {
                    break;
                  }
                  // Count backslashes.  Only a comma with an even number of
                  // backslashes or a single backslash preceded by a comma
                  // before it is recognized as a separator
                  if ((s > origval + 1 && s[-1] == '\\' && s[-2] != ',')
                      || (s == origval + 1 && s[-1] == '\\')) {
                    bs++;
                  } else {
                    bs = 0;
                  }
                }

                // do not add if already there
                if ((adding || prepending) && *s) {
                  prepending = FALSE;
                  adding = FALSE;
                  STRCPY(newval, origval);
                }
              }

              /* concatenate the two strings; add a ',' if
               * needed */
              if (adding || prepending) {
                comma = ((flags & P_COMMA) && *origval != NUL
                         && *newval != NUL);
                if (adding) {
                  i = (int)STRLEN(origval);
                  // Strip a trailing comma, would get 2.
                  if (comma && i > 1
                      && (flags & P_ONECOMMA) == P_ONECOMMA
                      && origval[i - 1] == ','
                      && origval[i - 2] != '\\') {
                    i--;
                  }
                  memmove(newval + i + comma, newval,
                      STRLEN(newval) + 1);
                  memmove(newval, origval, (size_t)i);
                } else {
                  i = (int)STRLEN(newval);
                  STRMOVE(newval + i + comma, origval);
                }
                if (comma)
                  newval[i] = ',';
              }

              /* Remove newval[] from origval[]. (Note: "i" has
               * been set above and is used here). */
              if (removing) {
                STRCPY(newval, origval);
                if (*s) {
                  /* may need to remove a comma */
                  if (flags & P_COMMA) {
                    if (s == origval) {
                      /* include comma after string */
                      if (s[i] == ',')
                        ++i;
                    } else {
                      /* include comma before string */
                      --s;
                      ++i;
                    }
                  }
                  STRMOVE(newval + (s - origval), s + i);
                }
              }

              if (flags & P_FLAGLIST) {
                // Remove flags that appear twice.
                for (s = newval; *s; s++) {
                  // if options have P_FLAGLIST and P_ONECOMMA such as
                  // 'whichwrap'
                  if (flags & P_ONECOMMA) {
                    if (*s != ',' && *(s + 1) == ','
                        && vim_strchr(s + 2, *s) != NULL) {
                      // Remove the duplicated value and the next comma.
                      STRMOVE(s, s + 2);
                      s -= 2;
                    }
                  } else {
                    if ((!(flags & P_COMMA) || *s != ',')
                        && vim_strchr(s + 1, *s) != NULL) {
                      STRMOVE(s, s + 1);
                      s--;
                    }
                  }
                }
              }

              if (save_arg != NULL)                 /* number for 'whichwrap' */
                arg = save_arg;
              new_value_alloced = TRUE;
            }

            /* Set the new value. */
            *(char_u **)(varp) = newval;

            if (!starting && origval != NULL) {
              // origval may be freed by
              // did_set_string_option(), make a copy.
              saved_origval = xstrdup((char *) origval);
            }

            /* Handle side effects, and set the global value for
             * ":set" on local options. */
            errmsg = did_set_string_option(opt_idx, (char_u **)varp,
                new_value_alloced, oldval, errbuf, opt_flags);

            // If error detected, print the error message.
            if (errmsg != NULL) {
              xfree(saved_origval);
              goto skip;
            }

            if (saved_origval != NULL) {
              char buf_type[7];
              vim_snprintf(buf_type, ARRAY_SIZE(buf_type), "%s",
                           (opt_flags & OPT_LOCAL) ? "local" : "global");
              set_vim_var_string(VV_OPTION_NEW, *(char **) varp, -1);
              set_vim_var_string(VV_OPTION_OLD, saved_origval, -1);
              set_vim_var_string(VV_OPTION_TYPE, buf_type, -1);
              apply_autocmds(EVENT_OPTIONSET,
                             (char_u *)options[opt_idx].fullname,
                             NULL, false, NULL);
              reset_v_option_vars();
              xfree(saved_origval);
            }
          } else {
            // key code option(FIXME(tarruda): Show a warning or something
            // similar)
          }
        }

        if (opt_idx >= 0)
          did_set_option(opt_idx, opt_flags,
              !prepending && !adding && !removing);
      }

skip:
      /*
       * Advance to next argument.
       * - skip until a blank found, taking care of backslashes
       * - skip blanks
       * - skip one "=val" argument (for hidden options ":set gfn =xx")
       */
      for (i = 0; i < 2; ++i) {
        while (*arg != NUL && !ascii_iswhite(*arg))
          if (*arg++ == '\\' && *arg != NUL)
            ++arg;
        arg = skipwhite(arg);
        if (*arg != '=')
          break;
      }
    }

    if (errmsg != NULL) {
      STRLCPY(IObuff, _(errmsg), IOSIZE);
      i = (int)STRLEN(IObuff) + 2;
      if (i + (arg - startarg) < IOSIZE) {
        /* append the argument with the error */
        STRCAT(IObuff, ": ");
        assert(arg >= startarg);
        memmove(IObuff + i, startarg, (size_t)(arg - startarg));
        IObuff[i + (arg - startarg)] = NUL;
      }
      /* make sure all characters are printable */
      trans_characters(IObuff, IOSIZE);

      ++no_wait_return;         /* wait_return done later */
      emsg(IObuff);             /* show error highlighted */
      --no_wait_return;

      return FAIL;
    }

    arg = skipwhite(arg);
  }

theend:
  if (silent_mode && did_show) {
    /* After displaying option values in silent mode. */
    silent_mode = FALSE;
    info_message = TRUE;        /* use mch_msg(), not mch_errmsg() */
    msg_putchar('\n');
    ui_flush();
    silent_mode = TRUE;
    info_message = FALSE;       /* use mch_msg(), not mch_errmsg() */
  }

  return OK;
}