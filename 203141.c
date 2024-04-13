_gdata_service_secure_strdup (const gchar *str)
{
#ifdef HAVE_GNOME
	return gnome_keyring_memory_strdup (str);
#else /* if !HAVE_GNOME */
	return g_strdup (str);
#endif /* !HAVE_GNOME */
}