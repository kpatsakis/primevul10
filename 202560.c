restore_backup_configuration_without_messages (const char *backup_filename, const char *intended_filename)
{
        backup_filename = gnome_rr_config_get_backup_filename ();
        rename (backup_filename, intended_filename);
}