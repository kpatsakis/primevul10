cr_tknzr_new_from_uri (const guchar * a_file_uri, 
                       enum CREncoding a_enc)
{
        CRTknzr *result = NULL;
        CRInput *input = NULL;

        input = cr_input_new_from_uri ((const gchar *) a_file_uri, a_enc);
        g_return_val_if_fail (input != NULL, NULL);

        result = cr_tknzr_new (input);

        return result;
}