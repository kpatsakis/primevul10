void set_init_3(void)
{
  // Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
  // This is done after other initializations, where 'shell' might have been
  // set, but only if they have not been set before.
  int idx_srr;
  int do_srr;
  int idx_sp;
  int do_sp;

  idx_srr = findoption((char_u *)"srr");
  if (idx_srr < 0)
    do_srr = FALSE;
  else
    do_srr = !(options[idx_srr].flags & P_WAS_SET);
  idx_sp = findoption((char_u *)"sp");
  if (idx_sp < 0)
    do_sp = FALSE;
  else
    do_sp = !(options[idx_sp].flags & P_WAS_SET);

  size_t len = 0;
  char_u *p = (char_u *)invocation_path_tail(p_sh, &len);
  p = vim_strnsave(p, len);

  {
    /*
     * Default for p_sp is "| tee", for p_srr is ">".
     * For known shells it is changed here to include stderr.
     */
    if (       fnamecmp(p, "csh") == 0
               || fnamecmp(p, "tcsh") == 0
               ) {
      if (do_sp) {
        p_sp = (char_u *)"|& tee";
        options[idx_sp].def_val[VI_DEFAULT] = p_sp;
      }
      if (do_srr) {
        p_srr = (char_u *)">&";
        options[idx_srr].def_val[VI_DEFAULT] = p_srr;
      }
    } else if (       fnamecmp(p, "sh") == 0
                      || fnamecmp(p, "ksh") == 0
                      || fnamecmp(p, "mksh") == 0
                      || fnamecmp(p, "pdksh") == 0
                      || fnamecmp(p, "zsh") == 0
                      || fnamecmp(p, "zsh-beta") == 0
                      || fnamecmp(p, "bash") == 0
                      || fnamecmp(p, "fish") == 0
                      ) {
      if (do_sp) {
        p_sp = (char_u *)"2>&1| tee";
        options[idx_sp].def_val[VI_DEFAULT] = p_sp;
      }
      if (do_srr) {
        p_srr = (char_u *)">%s 2>&1";
        options[idx_srr].def_val[VI_DEFAULT] = p_srr;
      }
    }
    xfree(p);
  }

  if (bufempty()) {
    int idx_ffs = findoption((char_u *)"ffs");

    // Apply the first entry of 'fileformats' to the initial buffer.
    if (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_SET)) {
      set_fileformat(default_fileformat(), OPT_LOCAL);
    }
  }

  set_title_defaults();
}