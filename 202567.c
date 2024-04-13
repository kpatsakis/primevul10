turn_on (GnomeRRScreen *screen,
         GnomeOutputInfo *info,
         int x, int y)
{
        GnomeRROutput *output =
                gnome_rr_screen_get_output_by_name (screen, info->name);
        GnomeRRMode *mode = gnome_rr_output_get_preferred_mode (output);

        if (mode) {
                info->on = TRUE;
                info->x = x;
                info->y = y;
                info->width = gnome_rr_mode_get_width (mode);
                info->height = gnome_rr_mode_get_height (mode);
                info->rotation = GNOME_RR_ROTATION_0;
                info->rate = gnome_rr_mode_get_freq (mode);

                return TRUE;
        }

        return FALSE;
}