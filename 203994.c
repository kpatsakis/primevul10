cr_tknzr_parse_atkeyword (CRTknzr * a_this, 
                          CRString ** a_str)
{
        guint32 cur_char = 0;
        CRInputPos init_pos;
        gboolean str_needs_free = FALSE;
        enum CRStatus status = CR_OK;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input
                              && a_str, CR_BAD_PARAM_ERROR);

        RECORD_INITIAL_POS (a_this, &init_pos);

        READ_NEXT_CHAR (a_this, &cur_char);

        if (cur_char != '@') {
                status = CR_PARSING_ERROR;
                goto error;
        }

        if (*a_str == NULL) {
                *a_str = cr_string_new ();
                str_needs_free = TRUE;
        }
        status = cr_tknzr_parse_ident (a_this, a_str);
        if (status != CR_OK) {
                goto error;
        }
        return CR_OK;
 error:

        if (str_needs_free == TRUE && *a_str) {
                cr_string_destroy (*a_str);
                *a_str = NULL;
        }
        cr_tknzr_set_cur_pos (a_this, &init_pos);
        return status;
}