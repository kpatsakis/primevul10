sanitize (GPtrArray *array)
{
        int i;
        GPtrArray *new;

        g_debug ("before sanitizing");

        for (i = 0; i < array->len; ++i) {
                if (array->pdata[i]) {
                        print_configuration (array->pdata[i], "before");
                }
        }


        /* Remove configurations that are duplicates of
         * configurations earlier in the cycle
         */
        for (i = 1; i < array->len; ++i) {
                int j;

                for (j = 0; j < i; ++j) {
                        GnomeRRConfig *this = array->pdata[j];
                        GnomeRRConfig *other = array->pdata[i];

                        if (this && other && gnome_rr_config_equal (this, other)) {
                                g_debug ("removing duplicate configuration");
                                gnome_rr_config_free (this);
                                array->pdata[j] = NULL;
                                break;
                        }
                }
        }

        for (i = 0; i < array->len; ++i) {
                GnomeRRConfig *config = array->pdata[i];

                if (config) {
                        gboolean all_off = TRUE;
                        int j;

                        for (j = 0; config->outputs[j] != NULL; ++j) {
                                if (config->outputs[j]->on)
                                        all_off = FALSE;
                        }

                        if (all_off) {
                                gnome_rr_config_free (array->pdata[i]);
                                array->pdata[i] = NULL;
                        }
                }
        }

        /* Remove NULL configurations */
        new = g_ptr_array_new ();

        for (i = 0; i < array->len; ++i) {
                if (array->pdata[i]) {
                        g_ptr_array_add (new, array->pdata[i]);
                        print_configuration (array->pdata[i], "Final");
                }
        }

        if (new->len > 0) {
                g_ptr_array_add (new, NULL);
        } else {
                g_ptr_array_free (new, TRUE);
                new = NULL;
        }

        g_ptr_array_free (array, TRUE);

        return new;
}