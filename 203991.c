cr_tknzr_parse_nmchar (CRTknzr * a_this, guint32 * a_char,
                       CRParsingLocation *a_location)
{
        guint32 cur_char = 0,
                next_char = 0;
        enum CRStatus status = CR_OK;
        CRInputPos init_pos;

        g_return_val_if_fail (a_this && PRIVATE (a_this) && a_char,
                              CR_BAD_PARAM_ERROR);

        RECORD_INITIAL_POS (a_this, &init_pos);

        status = cr_input_peek_char (PRIVATE (a_this)->input, 
                                     &next_char) ;
        if (status != CR_OK)
                goto error;

        if (next_char == '\\') {
                status = cr_tknzr_parse_escape (a_this, a_char, 
                                                a_location);

                if (status != CR_OK)
                        goto error;

        } else if (cr_utils_is_nonascii (next_char) == TRUE
                   || ((next_char >= 'a') && (next_char <= 'z'))
                   || ((next_char >= 'A') && (next_char <= 'Z'))
                   || ((next_char >= '0') && (next_char <= '9'))
                   || (next_char == '-')
                   || (next_char == '_') /*'_' not allowed by the spec. */
                ) {
                READ_NEXT_CHAR (a_this, &cur_char);
                *a_char = cur_char;
                status = CR_OK;
                if (a_location) {
                        cr_tknzr_get_parsing_location
                                (a_this, a_location) ;
                }
        } else {
                status = CR_PARSING_ERROR;
                goto error;
        }
        return CR_OK;

 error:
        cr_tknzr_set_cur_pos (a_this, &init_pos);
        return status;
}