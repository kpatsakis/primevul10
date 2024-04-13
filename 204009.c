cr_tknzr_new_from_buf (guchar * a_buf, gulong a_len,
                       enum CREncoding a_enc, 
                       gboolean a_free_at_destroy)
{
        CRTknzr *result = NULL;
        CRInput *input = NULL;

        input = cr_input_new_from_buf (a_buf, a_len, a_enc,
                                       a_free_at_destroy);

        g_return_val_if_fail (input != NULL, NULL);

        result = cr_tknzr_new (input);

        return result;
}