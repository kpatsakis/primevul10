static inline bool pcre_need_log_error(int pcre_code) {
  return RuntimeOption::EnablePregErrorLog &&
         (pcre_code == PCRE_ERROR_MATCHLIMIT ||
          pcre_code == PCRE_ERROR_RECURSIONLIMIT);
}