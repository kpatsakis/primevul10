set_optimize_exact(regex_t* reg, OptExact* e)
{
  int r;

  if (e->len == 0) return 0;

  if (e->ignore_case) {
    reg->exact = (UChar* )xmalloc(e->len);
    CHECK_NULL_RETURN_MEMERR(reg->exact);
    xmemcpy(reg->exact, e->s, e->len);
    reg->exact_end = reg->exact + e->len;
    reg->optimize = OPTIMIZE_STR_IC;
  }
  else {
    int allow_reverse;

    reg->exact = onigenc_strdup(reg->enc, e->s, e->s + e->len);
    CHECK_NULL_RETURN_MEMERR(reg->exact);
    reg->exact_end = reg->exact + e->len;

    allow_reverse =
      ONIGENC_IS_ALLOWED_REVERSE_MATCH(reg->enc, reg->exact, reg->exact_end);

    if (e->len >= 3 || (e->len >= 2 && allow_reverse)) {
#ifdef USE_SUNDAY_QUICK_SEARCH_ALGORITHM
      r = set_sunday_quick_search_skip_table(reg->exact, reg->exact_end,
                                             reg->enc, reg->map,
                                             &(reg->map_offset));
#else
      r = set_bmh_search_skip_table(reg->exact, reg->exact_end,
                                    reg->enc, reg->map);
#endif
      if (r != 0) return r;

      reg->optimize = (allow_reverse != 0
                       ? OPTIMIZE_STR_FAST
                       : OPTIMIZE_STR_FAST_STEP_FORWARD);
    }
    else {
      reg->optimize = OPTIMIZE_STR;
    }
  }

  reg->dmin = e->mmd.min;
  reg->dmax = e->mmd.max;

  if (reg->dmin != INFINITE_LEN) {
    reg->threshold_len = reg->dmin + (int )(reg->exact_end - reg->exact);
  }

  return 0;
}