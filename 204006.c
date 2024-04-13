cr_tknzr_parse_nmstart (CRTknzr * a_this, 
                        guint32 * a_char,
                        CRParsingLocation *a_location)
{
        CRInputPos init_pos;
        enum CRStatus status = CR_OK;
        guint32 cur_char = 0,
                next_char = 0;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input
                              && a_char, CR_BAD_PARAM_ERROR);

        RECORD_INITIAL_POS (a_this, &init_pos);

        PEEK_NEXT_CHAR (a_this, &next_char);

        if (next_char == '\\') {
                status = cr_tknzr_parse_escape (a_this, a_char,
                                                a_location);

                if (status != CR_OK)
                        goto error;

        } else if (cr_utils_is_nonascii (next_char) == TRUE
                   || ((next_char >= 'a') && (next_char <= 'z'))
                   || ((next_char >= 'A') && (next_char <= 'Z'))
                ) {
                READ_NEXT_CHAR (a_this, &cur_char);
                if (a_location) {
                        cr_tknzr_get_parsing_location (a_this, 
                                                       a_location) ;
                }
                *a_char = cur_char;
                status = CR_OK;
        } else {
                status = CR_PARSING_ERROR;
                goto error;
        }

        return CR_OK;

 error:        
        cr_tknzr_set_cur_pos (a_this, &init_pos);

        return status;

}