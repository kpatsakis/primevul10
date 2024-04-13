try_to_apply_intended_configuration (GsdXrandrManager *manager, GdkWindow *parent_window, guint32 timestamp, GError **error)
{
        char *backup_filename;
        char *intended_filename;
        gboolean result;

        /* Try to apply the intended configuration */

        backup_filename = gnome_rr_config_get_backup_filename ();
        intended_filename = gnome_rr_config_get_intended_filename ();

        result = apply_configuration_from_filename (manager, intended_filename, FALSE, timestamp, error);
        if (!result) {
                error_message (manager, _("The selected configuration for displays could not be applied"), error ? *error : NULL, NULL);
                restore_backup_configuration_without_messages (backup_filename, intended_filename);
                goto out;
        }

        /* Confirm with the user */

        if (user_says_things_are_ok (manager, parent_window))
                unlink (backup_filename);
        else
                restore_backup_configuration (manager, backup_filename, intended_filename, timestamp);

out:
        g_free (backup_filename);
        g_free (intended_filename);

        return result;
}