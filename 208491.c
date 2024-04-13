  FUNC_ATTR_NONNULL_ARG(2) FUNC_ATTR_WARN_UNUSED_RESULT
{
  if (options[opt_idx].var == NULL) {  // don't set hidden option
    return NULL;
  }

  char *const s = xstrdup(value);
  char **const varp = (char **)get_varp_scope(
      &(options[opt_idx]),
      ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
       ? (((int)options[opt_idx].indir & PV_BOTH)
          ? OPT_GLOBAL : OPT_LOCAL)
       : opt_flags));
  char *const oldval = *varp;
  *varp = s;

  char *const saved_oldval = (starting ? NULL : xstrdup(oldval));

  char *const r = (char *)did_set_string_option(
      opt_idx, (char_u **)varp, (int)true, (char_u *)oldval, NULL, opt_flags);
  if (r == NULL) {
    did_set_option(opt_idx, opt_flags, true);
  }

  // call autocommand after handling side effects
  if (saved_oldval != NULL) {
    char buf_type[7];
    vim_snprintf(buf_type, ARRAY_SIZE(buf_type), "%s",
                 (opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_NEW, (char *)(*varp), -1);
    set_vim_var_string(VV_OPTION_OLD, saved_oldval, -1);
    set_vim_var_string(VV_OPTION_TYPE, buf_type, -1);
    apply_autocmds(EVENT_OPTIONSET,
                   (char_u *)options[opt_idx].fullname,
                   NULL, false, NULL);
    reset_v_option_vars();
    xfree(saved_oldval);
  }

  return r;
}