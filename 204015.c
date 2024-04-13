cr_tknzr_parse_w (CRTknzr * a_this, 
                  guchar ** a_start, 
                  guchar ** a_end, 
                  CRParsingLocation *a_location)
{
        guint32 cur_char = 0;
        CRInputPos init_pos;
        enum CRStatus status = CR_OK;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input
                              && a_start && a_end, 
                              CR_BAD_PARAM_ERROR);

        RECORD_INITIAL_POS (a_this, &init_pos);

        *a_start = NULL;
        *a_end = NULL;

        READ_NEXT_CHAR (a_this, &cur_char);

        if (cr_utils_is_white_space (cur_char) == FALSE) {
                status = CR_PARSING_ERROR;
                goto error;
        }
        if (a_location) {
                cr_tknzr_get_parsing_location (a_this, 
                                               a_location) ;
        }
        RECORD_CUR_BYTE_ADDR (a_this, a_start);
        *a_end = *a_start;

        for (;;) {
                gboolean is_eof = FALSE;

                cr_input_get_end_of_file (PRIVATE (a_this)->input, &is_eof);
                if (is_eof)
                        break;

                status = cr_tknzr_peek_char (a_this, &cur_char);
                if (status == CR_END_OF_INPUT_ERROR) {
                        break;
                } else if (status != CR_OK) {
                        goto error;
                }

                if (cr_utils_is_white_space (cur_char) == TRUE) {
                        READ_NEXT_CHAR (a_this, &cur_char);
                        RECORD_CUR_BYTE_ADDR (a_this, a_end);
                } else {
                        break;
                }
        }

        return CR_OK;

      error:
        cr_tknzr_set_cur_pos (a_this, &init_pos);

        return status;
}