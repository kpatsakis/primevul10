cr_tknzr_parse_string (CRTknzr * a_this, CRString ** a_str)
{
        guint32 cur_char = 0,
                delim = 0;
        CRInputPos init_pos;
        enum CRStatus status = CR_OK;
        CRString *str = NULL;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input
                              && a_str, CR_BAD_PARAM_ERROR);

        RECORD_INITIAL_POS (a_this, &init_pos);
        READ_NEXT_CHAR (a_this, &cur_char);

        if (cur_char == '"')
                delim = '"';
        else if (cur_char == '\'')
                delim = '\'';
        else {
                status = CR_PARSING_ERROR;
                goto error;
        }
        str = cr_string_new ();
        if (str) {
                cr_tknzr_get_parsing_location 
                        (a_this, &str->location) ;
        }
        for (;;) {
                guchar next_chars[2] = { 0 };

                PEEK_BYTE (a_this, 1, &next_chars[0]);
                PEEK_BYTE (a_this, 2, &next_chars[1]);

                if (next_chars[0] == '\\') {
                        guchar *tmp_char_ptr1 = NULL,
                                *tmp_char_ptr2 = NULL;
                        guint32 esc_code = 0;

                        if (next_chars[1] == '\'' || next_chars[1] == '"') {
                                g_string_append_unichar (str->stryng, 
                                                         next_chars[1]);
                                SKIP_BYTES (a_this, 2);
                                status = CR_OK;
                        } else {
                                status = cr_tknzr_parse_escape
                                        (a_this, &esc_code, NULL);

                                if (status == CR_OK) {
                                        g_string_append_unichar
                                                (str->stryng, 
                                                 esc_code);
                                }
                        }

                        if (status != CR_OK) {
                                /*
                                 *consume the '\' char, and try to parse
                                 *a newline.
                                 */
                                READ_NEXT_CHAR (a_this, &cur_char);

                                status = cr_tknzr_parse_nl
                                        (a_this, &tmp_char_ptr1,
                                         &tmp_char_ptr2, NULL);
                        }

                        CHECK_PARSING_STATUS (status, FALSE);
                } else if (strchr ("\t !#$%&", next_chars[0])
                           || (next_chars[0] >= '(' && next_chars[0] <= '~')) {
                        READ_NEXT_CHAR (a_this, &cur_char);
                        g_string_append_unichar (str->stryng, 
                                                 cur_char);
                        status = CR_OK;
                }

                else if (cr_utils_is_nonascii (next_chars[0])) {
                        READ_NEXT_CHAR (a_this, &cur_char);
                        g_string_append_unichar (str->stryng, cur_char);
                } else if (next_chars[0] == delim) {
                        READ_NEXT_CHAR (a_this, &cur_char);
                        break;
                } else {
                        status = CR_PARSING_ERROR;
                        goto error;
                }
        }

        if (status == CR_OK) {
                if (*a_str == NULL) {
                        *a_str = str;
                        str = NULL;
                } else {
                        (*a_str)->stryng = g_string_append_len
                                ((*a_str)->stryng,
                                 str->stryng->str, 
                                 str->stryng->len);
                        cr_string_destroy (str);
                }
                return CR_OK;
        }

 error:

        if (str) {
                cr_string_destroy (str) ;
                str = NULL;
        }
        cr_tknzr_set_cur_pos (a_this, &init_pos);
        return status;
}