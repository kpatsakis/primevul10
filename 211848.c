static void init_local_extra(pcre_extra* local, pcre_extra* shared) {
  if (shared) {
    memcpy(local, shared, sizeof(pcre_extra));
  } else {
    memset(local, 0, sizeof(pcre_extra));
    local->flags = PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
  }
  local->match_limit = tl_pcre_globals->preg_backtrace_limit;
  local->match_limit_recursion = tl_pcre_globals->preg_recursion_limit;
}