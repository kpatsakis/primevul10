apply_intended_configuration (GsdXrandrManager *manager, const char *intended_filename, guint32 timestamp)
{
        GError *my_error;

        my_error = NULL;
        if (!apply_configuration_from_filename (manager, intended_filename, FALSE, timestamp, &my_error)) {
                if (my_error) {
                        if (!g_error_matches (my_error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
                                error_message (manager, _("Could not apply the stored configuration for monitors"), my_error, NULL);

                        g_error_free (my_error);
                }
        }
}