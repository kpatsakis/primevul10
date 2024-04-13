make_other_setup (GnomeRRScreen *screen)
{
        /* Turn off all laptops, and make all external monitors clone
         * from (0, 0)
         */

        GnomeRRConfig *result = gnome_rr_config_new_current (screen);
        int i;

        for (i = 0; result->outputs[i] != NULL; ++i) {
                GnomeOutputInfo *info = result->outputs[i];

                if (is_laptop (info)) {
                        info->on = FALSE;
                }
                else {
                        if (info->connected && !info->on) {
                                turn_on (screen, info, 0, 0);
                        }
               }
        }

        print_configuration (result, "other setup");

        return result;
}