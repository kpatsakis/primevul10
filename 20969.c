_g_file_set_attributes_from_entry (GFile                *file,
				   struct archive_entry *entry,
				   ExtractData          *extract_data,
				   GCancellable         *cancellable)
{
	GFileInfo *info;
	GError    *error = NULL;

	info = g_file_info_new ();

	/* times */

	if (archive_entry_ctime_is_set (entry)) {
		g_file_info_set_attribute_uint64 (info, G_FILE_ATTRIBUTE_TIME_CREATED, archive_entry_ctime (entry));
		g_file_info_set_attribute_uint32 (info, G_FILE_ATTRIBUTE_TIME_CREATED_USEC, archive_entry_ctime_nsec (entry));
	}

	if (archive_entry_mtime_is_set (entry)) {
		g_file_info_set_attribute_uint64 (info, G_FILE_ATTRIBUTE_TIME_MODIFIED, archive_entry_mtime (entry));
		g_file_info_set_attribute_uint32 (info, G_FILE_ATTRIBUTE_TIME_MODIFIED_USEC, archive_entry_mtime_nsec (entry));
	}

	/* username */

	if (archive_entry_uname (entry) != NULL) {
		guint32 uid;

		uid = GPOINTER_TO_INT (g_hash_table_lookup (extract_data->usernames, archive_entry_uname (entry)));
		if (uid == 0) {
			struct passwd *pwd = getpwnam (archive_entry_uname (entry));
			if (pwd != NULL) {
				uid = pwd->pw_uid;
				g_hash_table_insert (extract_data->usernames, g_strdup (archive_entry_uname (entry)), GINT_TO_POINTER (uid));
			}
		}
		if (uid != 0)
			g_file_info_set_attribute_uint32 (info, G_FILE_ATTRIBUTE_UNIX_UID, uid);
	}

	/* groupname */

	if (archive_entry_gname (entry) != NULL) {
		guint32 gid;

		gid = GPOINTER_TO_INT (g_hash_table_lookup (extract_data->groupnames, archive_entry_gname (entry)));
		if (gid == 0) {
			struct group *grp = getgrnam (archive_entry_gname (entry));
			if (grp != NULL) {
				gid = grp->gr_gid;
				g_hash_table_insert (extract_data->groupnames, g_strdup (archive_entry_gname (entry)), GINT_TO_POINTER (gid));
			}
		}
		if (gid != 0)
			g_file_info_set_attribute_uint32 (info, G_FILE_ATTRIBUTE_UNIX_GID, gid);
	}

	/* permsissions */

	g_file_info_set_attribute_uint32 (info, G_FILE_ATTRIBUTE_UNIX_MODE, archive_entry_mode (entry));

	if (! g_file_set_attributes_from_info (file, info, 0, cancellable, &error)) {
		g_warning ("%s", error->message);
		g_error_free (error);
	}

	g_object_unref (info);
}