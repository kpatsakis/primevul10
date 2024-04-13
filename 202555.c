make_clone_setup (GnomeRRScreen *screen)
{
        GnomeRRConfig *result;
        int width, height;
        int i;

        if (!get_clone_size (screen, &width, &height))
                return NULL;

        result = gnome_rr_config_new_current (screen);

        for (i = 0; result->outputs[i] != NULL; ++i) {
                GnomeOutputInfo *info = result->outputs[i];

                info->on = FALSE;
                if (info->connected) {
                        GnomeRROutput *output =
                                gnome_rr_screen_get_output_by_name (screen, info->name);
                        GnomeRRMode **modes = gnome_rr_output_list_modes (output);
                        int j;
                        int best_rate = 0;

                        for (j = 0; modes[j] != NULL; ++j) {
                                GnomeRRMode *mode = modes[j];
                                int w, h;

                                w = gnome_rr_mode_get_width (mode);
                                h = gnome_rr_mode_get_height (mode);

                                if (w == width && h == height) {
                                        int r = gnome_rr_mode_get_freq (mode);
                                        if (r > best_rate)
                                                best_rate = r;
                                }
                        }

                        if (best_rate > 0) {
                                info->on = TRUE;
                                info->width = width;
                                info->height = height;
                                info->rate = best_rate;
                                info->rotation = GNOME_RR_ROTATION_0;
                                info->x = 0;
                                info->y = 0;
                        }
                }
        }

        print_configuration (result, "clone setup");

        return result;
}