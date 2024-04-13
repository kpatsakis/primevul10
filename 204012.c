cr_tknzr_parse_important (CRTknzr * a_this,
                          CRParsingLocation *a_location)
{
        guint32 cur_char = 0;
        CRInputPos init_pos;
        enum CRStatus status = CR_OK;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input,
                              CR_BAD_PARAM_ERROR);

        RECORD_INITIAL_POS (a_this, &init_pos);
        READ_NEXT_CHAR (a_this, &cur_char);
        ENSURE_PARSING_COND (cur_char == '!');
        if (a_location) {
                cr_tknzr_get_parsing_location (a_this, 
                                               a_location) ;
        }
        cr_tknzr_try_to_skip_spaces (a_this);

        if (BYTE (PRIVATE (a_this)->input, 1, NULL) == 'i'
            && BYTE (PRIVATE (a_this)->input, 2, NULL) == 'm'
            && BYTE (PRIVATE (a_this)->input, 3, NULL) == 'p'
            && BYTE (PRIVATE (a_this)->input, 4, NULL) == 'o'
            && BYTE (PRIVATE (a_this)->input, 5, NULL) == 'r'
            && BYTE (PRIVATE (a_this)->input, 6, NULL) == 't'
            && BYTE (PRIVATE (a_this)->input, 7, NULL) == 'a'
            && BYTE (PRIVATE (a_this)->input, 8, NULL) == 'n'
            && BYTE (PRIVATE (a_this)->input, 9, NULL) == 't') {
                SKIP_BYTES (a_this, 9);
                if (a_location) {
                        cr_tknzr_get_parsing_location (a_this,
                                                       a_location) ;
                }
                return CR_OK;
        } else {
                status = CR_PARSING_ERROR;
        }

 error:
        cr_tknzr_set_cur_pos (a_this, &init_pos);

        return status;
}