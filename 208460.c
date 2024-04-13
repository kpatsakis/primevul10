set_option_value (
    char_u *name,
    long number,
    char_u *string,
    int opt_flags                  /* OPT_LOCAL or 0 (both) */
)
{
  if (set_tty_option((char *)name, (char *)string)) {
    return NULL;
  }

  int opt_idx;
  char_u      *varp;

  opt_idx = findoption(name);
  if (opt_idx < 0)
    EMSG2(_("E355: Unknown option: %s"), name);
  else {
    uint32_t flags = options[opt_idx].flags;
    // Disallow changing some options in the sandbox
    if (sandbox > 0 && (flags & P_SECURE)) {
      EMSG(_(e_sandbox));
      return NULL;
    }
    if (flags & P_STRING) {
      const char *s = (const char *)string;
      if (s == NULL) {
        s = "";
      }
      return (char_u *)set_string_option(opt_idx, s, opt_flags);
    } else {
      varp = get_varp_scope(&(options[opt_idx]), opt_flags);
      if (varp != NULL) {       /* hidden option is not changed */
        if (number == 0 && string != NULL) {
          int idx;

          // Either we are given a string or we are setting option
          // to zero.
          for (idx = 0; string[idx] == '0'; idx++) {}
          if (string[idx] != NUL || idx == 0) {
            // There's another character after zeros or the string
            // is empty.  In both cases, we are trying to set a
            // num option using a string.
            EMSG3(_("E521: Number required: &%s = '%s'"),
                  name, string);
            return NULL;  // do nothing as we hit an error
          }
        }
        if (flags & P_NUM)
          return set_num_option(opt_idx, varp, number,
              NULL, 0, opt_flags);
        else
          return set_bool_option(opt_idx, varp, (int)number,
              opt_flags);
      }
    }
  }
  return NULL;
}