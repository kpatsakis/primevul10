make_xinerama_setup (GnomeRRScreen *screen)
{
        /* Turn on everything that has a preferred mode, and
         * position it from left to right
         */
        GnomeRRConfig *result = gnome_rr_config_new_current (screen);
        int i;
        int x;

        x = 0;
        for (i = 0; result->outputs[i] != NULL; ++i) {
                GnomeOutputInfo *info = result->outputs[i];

                if (is_laptop (info)) {
                        if (info->on || turn_on (screen, info, x, 0)) {
                                x += info->width;
                        }
                }
        }

        for (i = 0; result->outputs[i] != NULL; ++i) {
                GnomeOutputInfo *info = result->outputs[i];

                if (info->connected && !is_laptop (info)) {
                        if (info->on || turn_on (screen, info, x, 0)) {
                                x += info->width;
                        }
                }
        }

        print_configuration (result, "xinerama setup");

        return result;
}