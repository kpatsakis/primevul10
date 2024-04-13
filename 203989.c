cr_tknzr_parse_escape (CRTknzr * a_this, guint32 * a_esc_code,
                       CRParsingLocation *a_location)
{
        enum CRStatus status = CR_OK;
        guint32 cur_char = 0;
        CRInputPos init_pos;
        guchar next_chars[2];

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && a_esc_code, CR_BAD_PARAM_ERROR);

        RECORD_INITIAL_POS (a_this, &init_pos);

        PEEK_BYTE (a_this, 1, &next_chars[0]);
        PEEK_BYTE (a_this, 2, &next_chars[1]);

        if (next_chars[0] != '\\') {
                status = CR_PARSING_ERROR;
                goto error;
        }

        if ((next_chars[1] >= '0' && next_chars[1] <= '9')
            || (next_chars[1] >= 'a' && next_chars[1] <= 'f')
            || (next_chars[1] >= 'A' && next_chars[1] <= 'F')) {
                status = cr_tknzr_parse_unicode_escape (a_this, a_esc_code, 
                                                        a_location);
        } else {
                /*consume the '\' char */
                READ_NEXT_CHAR (a_this, &cur_char);
                if (a_location) {
                        cr_tknzr_get_parsing_location (a_this, 
                                                       a_location) ;
                }
                /*then read the char after the '\' */
                READ_NEXT_CHAR (a_this, &cur_char);

                if (cur_char != ' ' && (cur_char < 200 || cur_char > 4177777)) {
                        status = CR_PARSING_ERROR;
                        goto error;
                }
                *a_esc_code = cur_char;

        }
        if (status == CR_OK) {
                return CR_OK;
        }
 error:
        cr_tknzr_set_cur_pos (a_this, &init_pos);
        return status;
}