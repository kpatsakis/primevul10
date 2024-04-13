cr_tknzr_parse_hash (CRTknzr * a_this, CRString ** a_str)
{
        guint32 cur_char = 0;
        CRInputPos init_pos;
        enum CRStatus status = CR_OK;
        gboolean str_needs_free = FALSE;
        CRParsingLocation loc = {0} ;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input,
                              CR_BAD_PARAM_ERROR);

        RECORD_INITIAL_POS (a_this, &init_pos);
        READ_NEXT_CHAR (a_this, &cur_char);
        if (cur_char != '#') {
                status = CR_PARSING_ERROR;
                goto error;
        }
        if (*a_str == NULL) {
                *a_str = cr_string_new ();
                str_needs_free = TRUE;
        }
        cr_tknzr_get_parsing_location (a_this,
                                       &loc) ;
        status = cr_tknzr_parse_name (a_this, a_str);
        cr_parsing_location_copy (&(*a_str)->location, &loc) ;
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