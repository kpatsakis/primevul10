get_clone_size (GnomeRRScreen *screen, int *width, int *height)
{
        GnomeRRMode **modes = gnome_rr_screen_list_clone_modes (screen);
        int best_w, best_h;
        int i;

        best_w = 0;
        best_h = 0;

        for (i = 0; modes[i] != NULL; ++i) {
                GnomeRRMode *mode = modes[i];
                int w, h;

                w = gnome_rr_mode_get_width (mode);
                h = gnome_rr_mode_get_height (mode);

                if (w * h > best_w * best_h) {
                        best_w = w;
                        best_h = h;
                }
        }

        if (best_w > 0 && best_h > 0) {
                if (width)
                        *width = best_w;
                if (height)
                        *height = best_h;

                return TRUE;
        }

        return FALSE;
}