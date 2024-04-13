cr_tknzr_parse_nl (CRTknzr * a_this, 
                   guchar ** a_start, 
                   guchar ** a_end, 
                   CRParsingLocation *a_location)
{
        CRInputPos init_pos;
        guchar next_chars[2] = { 0 };
        enum CRStatus status = CR_PARSING_ERROR;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && a_start && a_end, CR_BAD_PARAM_ERROR);

        RECORD_INITIAL_POS (a_this, &init_pos);

        PEEK_BYTE (a_this, 1, &next_chars[0]);
        PEEK_BYTE (a_this, 2, &next_chars[1]);

        if ((next_chars[0] == '\r' && next_chars[1] == '\n')) {
                SKIP_BYTES (a_this, 1);
                if (a_location) {
                        cr_tknzr_get_parsing_location 
                                (a_this, a_location) ;
                }
                SKIP_CHARS (a_this, 1);

                RECORD_CUR_BYTE_ADDR (a_this, a_end);

                status = CR_OK;
        } else if (next_chars[0] == '\n'
                   || next_chars[0] == '\r' || next_chars[0] == '\f') {
                SKIP_CHARS (a_this, 1);
                if (a_location) {
                        cr_tknzr_get_parsing_location 
                                (a_this, a_location) ;
                }
                RECORD_CUR_BYTE_ADDR (a_this, a_start);
                *a_end = *a_start;
                status = CR_OK;
        } else {
                status = CR_PARSING_ERROR;
                goto error;
        }
        return CR_OK ;

 error:
        cr_tknzr_set_cur_pos (a_this, &init_pos) ;
        return status;
}