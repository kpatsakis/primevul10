_gdata_service_get_log_level (void)
{
	static int level = -1;

	if (level < 0) {
		const gchar *envvar = g_getenv ("LIBGDATA_DEBUG");
		if (envvar != NULL)
			level = atoi (envvar);
		level = MIN (MAX (level, 0), GDATA_LOG_FULL);
	}

	return level;
}