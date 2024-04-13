debug_handler (const char *log_domain, GLogLevelFlags log_level, const char *message, gpointer user_data)
{
	if (_gdata_service_get_log_level () != GDATA_LOG_NONE)
		g_log_default_handler (log_domain, log_level, message, NULL);
}