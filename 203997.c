cr_tknzr_new (CRInput * a_input)
{
        CRTknzr *result = NULL;

        result = g_try_malloc (sizeof (CRTknzr));

        if (result == NULL) {
                cr_utils_trace_info ("Out of memory");
                return NULL;
        }

        memset (result, 0, sizeof (CRTknzr));

        result->priv = g_try_malloc (sizeof (CRTknzrPriv));

        if (result->priv == NULL) {
                cr_utils_trace_info ("Out of memory");

                if (result) {
                        g_free (result);
                        result = NULL;
                }

                return NULL;
        }
        memset (result->priv, 0, sizeof (CRTknzrPriv));
        if (a_input)
                cr_tknzr_set_input (result, a_input);
        return result;
}