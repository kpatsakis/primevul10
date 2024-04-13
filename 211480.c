static void pcre_log_error(const char* func, int line, int pcre_code,
                           const char* pattern, int pattern_size,
                           const char* subject, int subject_size,
                           const char* repl, int repl_size,
                           int arg1 = 0, int arg2 = 0,
                           int arg3 = 0, int arg4 = 0) {
  const char* escapedPattern;
  const char* escapedSubject;
  const char* escapedRepl;
  std::string p(pattern, pattern_size);
  std::string s(subject, subject_size);
  std::string r(repl, repl_size);
  escapedPattern = Logger::EscapeString(p);
  escapedSubject = Logger::EscapeString(s);
  escapedRepl = Logger::EscapeString(r);
  const char* errString =
    (pcre_code == PCRE_ERROR_MATCHLIMIT) ? "PCRE_ERROR_MATCHLIMIT" :
    (pcre_code == PCRE_ERROR_RECURSIONLIMIT) ? "PCRE_ERROR_RECURSIONLIMIT" :
    "UNKNOWN";
  raise_warning_unsampled(
    "REGEXERR: %s/%d: err=%d(%s), pattern='%s', subject='%s', repl='%s', "
    "limits=(%" PRId64 ", %" PRId64 "), extra=(%d, %d, %d, %d)",
    func, line, pcre_code, errString,
    escapedPattern, escapedSubject, escapedRepl,
    tl_pcre_globals->preg_backtrace_limit,
    tl_pcre_globals->preg_recursion_limit,
    arg1, arg2, arg3, arg4);
  free((void *)escapedPattern);
  free((void *)escapedSubject);
  free((void *)escapedRepl);
}