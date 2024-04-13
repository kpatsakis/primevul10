cr_input_new_from_buf (guchar * a_buf,
                       gulong a_len,
                       enum CREncoding a_enc,
                       gboolean a_free_buf)
{
        CRInput *result = NULL;
        enum CRStatus status = CR_OK;
        CREncHandler *enc_handler = NULL;
        gulong len = a_len;

        g_return_val_if_fail (a_buf, NULL);

        result = cr_input_new_real ();
        g_return_val_if_fail (result, NULL);

        /*transform the encoding in utf8 */
        if (a_enc != CR_UTF_8) {
                enc_handler = cr_enc_handler_get_instance (a_enc);
                if (!enc_handler) {
                        goto error;
                }

                status = cr_enc_handler_convert_input
                        (enc_handler, a_buf, &len,
                         &PRIVATE (result)->in_buf,
                         &PRIVATE (result)->in_buf_size);
                if (status != CR_OK)
                        goto error;
                PRIVATE (result)->free_in_buf = TRUE;
                if (a_free_buf == TRUE && a_buf) {
                        g_free (a_buf) ;
                        a_buf = NULL ;
                }                
                PRIVATE (result)->nb_bytes = PRIVATE (result)->in_buf_size;
        } else {
                PRIVATE (result)->in_buf = (guchar *) a_buf;
                PRIVATE (result)->in_buf_size = a_len;
                PRIVATE (result)->nb_bytes = a_len;
                PRIVATE (result)->free_in_buf = a_free_buf;
        }
        PRIVATE (result)->line = 1;
        PRIVATE (result)->col =  0;
        return result;

 error:
        if (result) {
                cr_input_destroy (result);
                result = NULL;
        }

        return NULL;
}