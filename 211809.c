PregWithErrorGuard::~PregWithErrorGuard() {
  if (*rl_last_error_code == PHP_PCRE_NO_ERROR) {
    error.setNull();
  } else {
    error = *rl_last_error_code;
  }
  *rl_last_error_code = prior_error;
}