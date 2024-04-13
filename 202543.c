get_allowed_rotations_for_output (GsdXrandrManager *manager, GnomeOutputInfo *output, int *out_num_rotations, GnomeRRRotation *out_rotations)
{
        static const GnomeRRRotation possible_rotations[] = {
                GNOME_RR_ROTATION_0,
                GNOME_RR_ROTATION_90,
                GNOME_RR_ROTATION_180,
                GNOME_RR_ROTATION_270
                /* We don't allow REFLECT_X or REFLECT_Y for now, as gnome-display-properties doesn't allow them, either */
        };

        struct GsdXrandrManagerPrivate *priv = manager->priv;
        GnomeRRRotation current_rotation;
        int i;

        *out_num_rotations = 0;
        *out_rotations = 0;

        current_rotation = output->rotation;

        /* Yay for brute force */

        for (i = 0; i < G_N_ELEMENTS (possible_rotations); i++) {
                GnomeRRRotation rotation_to_test;

                rotation_to_test = possible_rotations[i];

                output->rotation = rotation_to_test;

                if (gnome_rr_config_applicable (priv->configuration, priv->rw_screen, NULL)) { /* NULL-GError */
                        (*out_num_rotations)++;
                        (*out_rotations) |= rotation_to_test;
                }
        }

        output->rotation = current_rotation;

        if (*out_num_rotations == 0 || *out_rotations == 0) {
                g_warning ("Huh, output %p says it doesn't support any rotations, and yet it has a current rotation?", output);
                *out_num_rotations = 1;
                *out_rotations = output->rotation;
        }
}