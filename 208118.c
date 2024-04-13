pam_settrace()
{
	void	*defp;

	if ((defp = defopen_r(PAM_DEBUG)) != NULL) {
		char	*arg;
		int	code;
		int	facility = LOG_AUTH;

		pam_debug = PAM_DEBUG_DEFAULT;
		log_priority = LOG_DEBUG;

		(void) defcntl_r(DC_SETFLAGS, DC_CASE, defp);
		if ((arg = defread_r(LOG_PRIORITY, defp)) != NULL) {
			code = (int)strtol(arg, NULL, 10);
			if ((code & ~LOG_PRIMASK) == 0) {
				log_priority = code;
			}
		}
		if ((arg = defread_r(LOG_FACILITY, defp)) != NULL) {
			code = (int)strtol(arg, NULL, 10);
			if (code < LOG_NFACILITIES) {
				facility = code << 3;
			}
		}
		if ((arg = defread_r(DEBUG_FLAGS, defp)) != NULL) {
			pam_debug = (int)strtol(arg, NULL, 0);
		}
		defclose_r(defp);

		log_priority |= facility;
	}
}