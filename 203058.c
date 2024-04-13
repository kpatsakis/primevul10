empathy_adium_path_is_valid (const gchar *path)
{
	gboolean ret;
	gchar   *file;

	/* The theme is not valid if there is no Info.plist */
	file = g_build_filename (path, "Contents", "Info.plist",
				 NULL);
	ret = g_file_test (file, G_FILE_TEST_EXISTS);
	g_free (file);

	if (!ret)
		return FALSE;

	/* We ship a default Template.html as fallback if there is any problem
	 * with the one inside the theme. The only other required file is
	 * Content.html OR Incoming/Content.html*/
	file = g_build_filename (path, "Contents", "Resources", "Content.html",
				 NULL);
	ret = g_file_test (file, G_FILE_TEST_EXISTS);
	g_free (file);

	if (!ret) {
		file = g_build_filename (path, "Contents", "Resources", "Incoming",
					 "Content.html", NULL);
		ret = g_file_test (file, G_FILE_TEST_EXISTS);
		g_free (file);
	}

	return ret;
}