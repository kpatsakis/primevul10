cr_tknzr_parse_unicode_escape (CRTknzr * a_this, 
                               guint32 * a_unicode,
                               CRParsingLocation *a_location)
{
        guint32 cur_char;
        CRInputPos init_pos;
        glong occur = 0;
        guint32 unicode = 0;
        guchar *tmp_char_ptr1 = NULL,
                *tmp_char_ptr2 = NULL;
        enum CRStatus status = CR_OK;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && a_unicode, CR_BAD_PARAM_ERROR);

        /*first, let's backup the current position pointer */
        RECORD_INITIAL_POS (a_this, &init_pos);

        READ_NEXT_CHAR (a_this, &cur_char);

        if (cur_char != '\\') {
                status = CR_PARSING_ERROR;
                goto error;
        }
        if (a_location) {
                cr_tknzr_get_parsing_location 
                        (a_this, a_location) ;
        }
        PEEK_NEXT_CHAR (a_this, &cur_char);

        for (occur = 0, unicode = 0; ((cur_char >= '0' && cur_char <= '9')
                                      || (cur_char >= 'a' && cur_char <= 'f')
                                      || (cur_char >= 'A' && cur_char <= 'F'))
             && occur < 6; occur++) {
                gint cur_char_val = 0;

                READ_NEXT_CHAR (a_this, &cur_char);

                if ((cur_char >= '0' && cur_char <= '9')) {
                        cur_char_val = (cur_char - '0');
                } else if ((cur_char >= 'a' && cur_char <= 'f')) {
                        cur_char_val = 10 + (cur_char - 'a');
                } else if ((cur_char >= 'A' && cur_char <= 'F')) {
                        cur_char_val = 10 + (cur_char - 'A');
                }

                unicode = unicode * 16 + cur_char_val;

                PEEK_NEXT_CHAR (a_this, &cur_char);
        }

        /* Eat a whitespace if possible. */
        cr_tknzr_parse_w (a_this, &tmp_char_ptr1, 
                          &tmp_char_ptr2, NULL);
        *a_unicode = unicode;
        return CR_OK;

      error:
        /*
         *restore the initial position pointer backuped at
         *the beginning of this function.
         */
        cr_tknzr_set_cur_pos (a_this, &init_pos);

        return status;
}