static void set_string_default(const char *name, char *val, bool allocated)
  FUNC_ATTR_NONNULL_ALL
{
  int opt_idx = findoption((char_u *)name);
  if (opt_idx >= 0) {
    if (options[opt_idx].flags & P_DEF_ALLOCED) {
      xfree(options[opt_idx].def_val[VI_DEFAULT]);
    }

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *) (
        allocated
        ? (char_u *) val
        : (char_u *) xstrdup(val));
    options[opt_idx].flags |= P_DEF_ALLOCED;
  }
}