void buf_copy_options(buf_T *buf, int flags)
{
  int should_copy = TRUE;
  char_u      *save_p_isk = NULL;           /* init for GCC */
  int dont_do_help;
  int did_isk = FALSE;

  /*
   * Don't do anything if the buffer is invalid.
   */
  if (buf == NULL || !buf_valid(buf))
    return;

  /*
   * Skip this when the option defaults have not been set yet.  Happens when
   * main() allocates the first buffer.
   */
  if (p_cpo != NULL) {
    /*
     * Always copy when entering and 'cpo' contains 'S'.
     * Don't copy when already initialized.
     * Don't copy when 'cpo' contains 's' and not entering.
     * 'S'	BCO_ENTER  initialized	's'  should_copy
     * yes	  yes	       X	 X	TRUE
     * yes	  no	      yes	 X	FALSE
     * no	   X	      yes	 X	FALSE
     *  X	  no	      no	yes	FALSE
     *  X	  no	      no	no	TRUE
     * no	  yes	      no	 X	TRUE
     */
    if ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == NULL || !(flags & BCO_ENTER))
        && (buf->b_p_initialized
            || (!(flags & BCO_ENTER)
                && vim_strchr(p_cpo, CPO_BUFOPT) != NULL)))
      should_copy = FALSE;

    if (should_copy || (flags & BCO_ALWAYS)) {
      /* Don't copy the options specific to a help buffer when
      * BCO_NOHELP is given or the options were initialized already
      * (jumping back to a help file with CTRL-T or CTRL-O) */
      dont_do_help = ((flags & BCO_NOHELP) && buf->b_help)
                     || buf->b_p_initialized;
      if (dont_do_help) {               /* don't free b_p_isk */
        save_p_isk = buf->b_p_isk;
        buf->b_p_isk = NULL;
      }
      /*
       * Always free the allocated strings.
       * If not already initialized, set 'readonly' and copy 'fileformat'.
       */
      if (!buf->b_p_initialized) {
        free_buf_options(buf, TRUE);
        buf->b_p_ro = FALSE;                    /* don't copy readonly */
        buf->b_p_fenc = vim_strsave(p_fenc);
        buf->b_p_ff = vim_strsave(p_ff);
        buf->b_p_bh = empty_option;
        buf->b_p_bt = empty_option;
      } else
        free_buf_options(buf, FALSE);

      buf->b_p_ai = p_ai;
      buf->b_p_ai_nopaste = p_ai_nopaste;
      buf->b_p_sw = p_sw;
      buf->b_p_tw = p_tw;
      buf->b_p_tw_nopaste = p_tw_nopaste;
      buf->b_p_tw_nobin = p_tw_nobin;
      buf->b_p_wm = p_wm;
      buf->b_p_wm_nopaste = p_wm_nopaste;
      buf->b_p_wm_nobin = p_wm_nobin;
      buf->b_p_bin = p_bin;
      buf->b_p_bomb = p_bomb;
      buf->b_p_et = p_et;
      buf->b_p_fixeol = p_fixeol;
      buf->b_p_et_nobin = p_et_nobin;
      buf->b_p_et_nopaste = p_et_nopaste;
      buf->b_p_ml = p_ml;
      buf->b_p_ml_nobin = p_ml_nobin;
      buf->b_p_inf = p_inf;
      buf->b_p_swf = p_swf;
      buf->b_p_cpt = vim_strsave(p_cpt);
      buf->b_p_cfu = vim_strsave(p_cfu);
      buf->b_p_ofu = vim_strsave(p_ofu);
      buf->b_p_sts = p_sts;
      buf->b_p_sts_nopaste = p_sts_nopaste;
      buf->b_p_com = vim_strsave(p_com);
      buf->b_p_cms = vim_strsave(p_cms);
      buf->b_p_fo = vim_strsave(p_fo);
      buf->b_p_flp = vim_strsave(p_flp);
      buf->b_p_nf = vim_strsave(p_nf);
      buf->b_p_mps = vim_strsave(p_mps);
      buf->b_p_si = p_si;
      buf->b_p_ci = p_ci;
      buf->b_p_cin = p_cin;
      buf->b_p_cink = vim_strsave(p_cink);
      buf->b_p_cino = vim_strsave(p_cino);
      /* Don't copy 'filetype', it must be detected */
      buf->b_p_ft = empty_option;
      buf->b_p_pi = p_pi;
      buf->b_p_cinw = vim_strsave(p_cinw);
      buf->b_p_lisp = p_lisp;
      /* Don't copy 'syntax', it must be set */
      buf->b_p_syn = empty_option;
      buf->b_p_smc = p_smc;
      buf->b_s.b_syn_isk = empty_option;
      buf->b_s.b_p_spc = vim_strsave(p_spc);
      (void)compile_cap_prog(&buf->b_s);
      buf->b_s.b_p_spf = vim_strsave(p_spf);
      buf->b_s.b_p_spl = vim_strsave(p_spl);
      buf->b_p_inde = vim_strsave(p_inde);
      buf->b_p_indk = vim_strsave(p_indk);
      buf->b_p_fex = vim_strsave(p_fex);
      buf->b_p_sua = vim_strsave(p_sua);
      buf->b_p_keymap = vim_strsave(p_keymap);
      buf->b_kmap_state |= KEYMAP_INIT;
      /* This isn't really an option, but copying the langmap and IME
      * state from the current buffer is better than resetting it. */
      buf->b_p_iminsert = p_iminsert;
      buf->b_p_imsearch = p_imsearch;

      /* options that are normally global but also have a local value
       * are not copied, start using the global value */
      buf->b_p_ar = -1;
      buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
      buf->b_p_bkc = empty_option;
      buf->b_bkc_flags = 0;
      buf->b_p_gp = empty_option;
      buf->b_p_mp = empty_option;
      buf->b_p_efm = empty_option;
      buf->b_p_ep = empty_option;
      buf->b_p_kp = empty_option;
      buf->b_p_path = empty_option;
      buf->b_p_tags = empty_option;
      buf->b_p_tc = empty_option;
      buf->b_tc_flags = 0;
      buf->b_p_def = empty_option;
      buf->b_p_inc = empty_option;
      buf->b_p_inex = vim_strsave(p_inex);
      buf->b_p_dict = empty_option;
      buf->b_p_tsr = empty_option;
      buf->b_p_qe = vim_strsave(p_qe);
      buf->b_p_udf = p_udf;
      buf->b_p_lw = empty_option;

      /*
       * Don't copy the options set by ex_help(), use the saved values,
       * when going from a help buffer to a non-help buffer.
       * Don't touch these at all when BCO_NOHELP is used and going from
       * or to a help buffer.
       */
      if (dont_do_help)
        buf->b_p_isk = save_p_isk;
      else {
        buf->b_p_isk = vim_strsave(p_isk);
        did_isk = true;
        buf->b_p_ts = p_ts;
        buf->b_help = false;
        if (buf->b_p_bt[0] == 'h')
          clear_string_option(&buf->b_p_bt);
        buf->b_p_ma = p_ma;
      }
    }

    /*
     * When the options should be copied (ignoring BCO_ALWAYS), set the
     * flag that indicates that the options have been initialized.
     */
    if (should_copy)
      buf->b_p_initialized = true;
  }

  check_buf_options(buf);           /* make sure we don't have NULLs */
  if (did_isk)
    (void)buf_init_chartab(buf, FALSE);
}