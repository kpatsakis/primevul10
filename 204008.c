cr_tknzr_parse_uri (CRTknzr * a_this, 
                    CRString ** a_str)
{
        guint32 cur_char = 0;
        CRInputPos init_pos;
        enum CRStatus status = CR_PARSING_ERROR;
        guchar tab[4] = { 0 }, *tmp_ptr1 = NULL, *tmp_ptr2 = NULL;
        CRString *str = NULL;
        CRParsingLocation location = {0} ;

        g_return_val_if_fail (a_this 
                              && PRIVATE (a_this)
                              && PRIVATE (a_this)->input
                              && a_str, 
                              CR_BAD_PARAM_ERROR);

        RECORD_INITIAL_POS (a_this, &init_pos);

        PEEK_BYTE (a_this, 1, &tab[0]);
        PEEK_BYTE (a_this, 2, &tab[1]);
        PEEK_BYTE (a_this, 3, &tab[2]);
        PEEK_BYTE (a_this, 4, &tab[3]);

        if (tab[0] != 'u' || tab[1] != 'r' || tab[2] != 'l' || tab[3] != '(') {
                status = CR_PARSING_ERROR;
                goto error;
        }
        /*
         *Here, we want to skip 4 bytes ('u''r''l''(').
         *But we also need to keep track of the parsing location
         *of the 'u'. So, we skip 1 byte, we record the parsing
         *location, then we skip the 3 remaining bytes.
         */
        SKIP_CHARS (a_this, 1);
        cr_tknzr_get_parsing_location (a_this, &location) ;
        SKIP_CHARS (a_this, 3);
        cr_tknzr_try_to_skip_spaces (a_this);
        status = cr_tknzr_parse_string (a_this, a_str);

        if (status == CR_OK) {
                guint32 next_char = 0;
                status = cr_tknzr_parse_w (a_this, &tmp_ptr1, 
                                           &tmp_ptr2, NULL);
                cr_tknzr_try_to_skip_spaces (a_this);
                PEEK_NEXT_CHAR (a_this, &next_char);
                if (next_char == ')') {
                        READ_NEXT_CHAR (a_this, &cur_char);
                        status = CR_OK;
                } else {
                        status = CR_PARSING_ERROR;
                }
        }
        if (status != CR_OK) {
                str = cr_string_new ();
                for (;;) {
                        guint32 next_char = 0;
                        PEEK_NEXT_CHAR (a_this, &next_char);
                        if (strchr ("!#$%&", next_char)
                            || (next_char >= '*' && next_char <= '~')
                            || (cr_utils_is_nonascii (next_char) == TRUE)) {
                                READ_NEXT_CHAR (a_this, &cur_char);
                                g_string_append_unichar 
                                        (str->stryng, cur_char);
                                status = CR_OK;
                        } else {
                                guint32 esc_code = 0;
                                status = cr_tknzr_parse_escape
                                        (a_this, &esc_code, NULL);
                                if (status == CR_OK) {
                                        g_string_append_unichar
                                                (str->stryng, 
                                                 esc_code);
                                } else {
                                        status = CR_OK;
                                        break;
                                }
                        }
                }
                cr_tknzr_try_to_skip_spaces (a_this);
                READ_NEXT_CHAR (a_this, &cur_char);
                if (cur_char == ')') {
                        status = CR_OK;
                } else {
                        status = CR_PARSING_ERROR;
                        goto error;
                }
                if (str) {                        
                        if (*a_str == NULL) {
                                *a_str = str;
                                str = NULL;
                        } else {
                                g_string_append_len
                                        ((*a_str)->stryng,
                                         str->stryng->str,
                                         str->stryng->len);
                                cr_string_destroy (str);
                        }                        
                }
        }

        cr_parsing_location_copy
                (&(*a_str)->location,
                 &location) ;
        return CR_OK ;
 error:
        if (str) {
                cr_string_destroy (str);
                str = NULL;
        }
        cr_tknzr_set_cur_pos (a_this, &init_pos);
        return status;
}