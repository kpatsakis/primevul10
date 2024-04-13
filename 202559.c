print_configuration (GnomeRRConfig *config, const char *header)
{
        int i;

        g_print ("=== %s Configuration ===\n", header);
        if (!config) {
                g_print ("  none\n");
                return;
        }

        for (i = 0; config->outputs[i] != NULL; ++i)
                print_output (config->outputs[i]);
}