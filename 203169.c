_gdata_service_secure_strfree (GDataSecureString str)
{
#ifdef HAVE_GNOME
	gnome_keyring_memory_free (str);
#else /* if !HAVE_GNOME */
	/* Poor man's approximation to non-pageable memory: the best we can do is ensure that we don't leak it in free memory.
	 * This can't guarantee that it hasn't hit disk at some point, but does mean it can't hit disk in future. */
	if (str != NULL) {
		memset (str, 0, strlen (str));
	}

	g_free (str);
#endif /* !HAVE_GNOME */
}