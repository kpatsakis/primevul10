cr_input_new_real (void)
{
        CRInput *result = NULL;

        result = g_try_malloc (sizeof (CRInput));
        if (!result) {
                cr_utils_trace_info ("Out of memory");
                return NULL;
        }
        memset (result, 0, sizeof (CRInput));

        PRIVATE (result) = g_try_malloc (sizeof (CRInputPriv));
        if (!PRIVATE (result)) {
                cr_utils_trace_info ("Out of memory");
                g_free (result);
                return NULL;
        }
        memset (PRIVATE (result), 0, sizeof (CRInputPriv));
        PRIVATE (result)->free_in_buf = TRUE;
        return result;
}