cr_tknzr_get_next_token (CRTknzr * a_this, CRToken ** a_tk)
{
        enum CRStatus status = CR_OK;
        CRToken *token = NULL;
        CRInputPos init_pos;
        guint32 next_char = 0;
        guchar next_bytes[4] = { 0 };
        gboolean reached_eof = FALSE;
        CRInput *input = NULL;
        CRString *str = NULL;
        CRRgb *rgb = NULL;
        CRParsingLocation location = {0} ;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && a_tk && *a_tk == NULL
                              && PRIVATE (a_this)->input, 
                              CR_BAD_PARAM_ERROR);

        if (PRIVATE (a_this)->token_cache) {
                *a_tk = PRIVATE (a_this)->token_cache;
                PRIVATE (a_this)->token_cache = NULL;
                return CR_OK;
        }

        RECORD_INITIAL_POS (a_this, &init_pos);

        status = cr_input_get_end_of_file
                (PRIVATE (a_this)->input, &reached_eof);
        ENSURE_PARSING_COND (status == CR_OK);

        if (reached_eof == TRUE) {
                status = CR_END_OF_INPUT_ERROR;
                goto error;
        }

        input = PRIVATE (a_this)->input;

        PEEK_NEXT_CHAR (a_this, &next_char);
        token = cr_token_new ();
        ENSURE_PARSING_COND (token);

        switch (next_char) {
        case '@':
                {
                        if (BYTE (input, 2, NULL) == 'f'
                            && BYTE (input, 3, NULL) == 'o'
                            && BYTE (input, 4, NULL) == 'n'
                            && BYTE (input, 5, NULL) == 't'
                            && BYTE (input, 6, NULL) == '-'
                            && BYTE (input, 7, NULL) == 'f'
                            && BYTE (input, 8, NULL) == 'a'
                            && BYTE (input, 9, NULL) == 'c'
                            && BYTE (input, 10, NULL) == 'e') {
                                SKIP_CHARS (a_this, 1);
                                cr_tknzr_get_parsing_location 
                                        (a_this, &location) ;
                                SKIP_CHARS (a_this, 9);
                                status = cr_token_set_font_face_sym (token);
                                CHECK_PARSING_STATUS (status, TRUE);
                                cr_parsing_location_copy (&token->location,
                                                          &location) ;
                                goto done;
                        }

                        if (BYTE (input, 2, NULL) == 'c'
                            && BYTE (input, 3, NULL) == 'h'
                            && BYTE (input, 4, NULL) == 'a'
                            && BYTE (input, 5, NULL) == 'r'
                            && BYTE (input, 6, NULL) == 's'
                            && BYTE (input, 7, NULL) == 'e'
                            && BYTE (input, 8, NULL) == 't') {
                                SKIP_CHARS (a_this, 1);
                                cr_tknzr_get_parsing_location
                                        (a_this, &location) ;
                                SKIP_CHARS (a_this, 7);
                                status = cr_token_set_charset_sym (token);
                                CHECK_PARSING_STATUS (status, TRUE);
                                cr_parsing_location_copy (&token->location,
                                                          &location) ;
                                goto done;
                        }

                        if (BYTE (input, 2, NULL) == 'i'
                            && BYTE (input, 3, NULL) == 'm'
                            && BYTE (input, 4, NULL) == 'p'
                            && BYTE (input, 5, NULL) == 'o'
                            && BYTE (input, 6, NULL) == 'r'
                            && BYTE (input, 7, NULL) == 't') {
                                SKIP_CHARS (a_this, 1);
                                cr_tknzr_get_parsing_location 
                                        (a_this, &location) ;
                                SKIP_CHARS (a_this, 6);
                                status = cr_token_set_import_sym (token);
                                CHECK_PARSING_STATUS (status, TRUE);
                                cr_parsing_location_copy (&token->location,
                                                          &location) ;
                                goto done;
                        }

                        if (BYTE (input, 2, NULL) == 'm'
                            && BYTE (input, 3, NULL) == 'e'
                            && BYTE (input, 4, NULL) == 'd'
                            && BYTE (input, 5, NULL) == 'i'
                            && BYTE (input, 6, NULL) == 'a') {
                                SKIP_CHARS (a_this, 1);
                                cr_tknzr_get_parsing_location (a_this, 
                                                               &location) ;
                                SKIP_CHARS (a_this, 5);
                                status = cr_token_set_media_sym (token);
                                CHECK_PARSING_STATUS (status, TRUE);
                                cr_parsing_location_copy (&token->location, 
                                                          &location) ;
                                goto done;
                        }

                        if (BYTE (input, 2, NULL) == 'p'
                            && BYTE (input, 3, NULL) == 'a'
                            && BYTE (input, 4, NULL) == 'g'
                            && BYTE (input, 5, NULL) == 'e') {
                                SKIP_CHARS (a_this, 1);
                                cr_tknzr_get_parsing_location (a_this, 
                                                               &location) ;
                                SKIP_CHARS (a_this, 4);
                                status = cr_token_set_page_sym (token);
                                CHECK_PARSING_STATUS (status, TRUE);
                                cr_parsing_location_copy (&token->location, 
                                                          &location) ;
                                goto done;
                        }
                        status = cr_tknzr_parse_atkeyword (a_this, &str);
                        if (status == CR_OK) {
                                status = cr_token_set_atkeyword (token, str);
                                CHECK_PARSING_STATUS (status, TRUE);
                                if (str) {
                                        cr_parsing_location_copy (&token->location, 
                                                                  &str->location) ;
                                }
                                goto done;
                        }
                }
                break;

        case 'u':

                if (BYTE (input, 2, NULL) == 'r'
                    && BYTE (input, 3, NULL) == 'l'
                    && BYTE (input, 4, NULL) == '(') {
                        CRString *str2 = NULL;

                        status = cr_tknzr_parse_uri (a_this, &str2);
                        if (status == CR_OK) {
                                status = cr_token_set_uri (token, str2);
                                CHECK_PARSING_STATUS (status, TRUE);
                                if (str2) {
                                        cr_parsing_location_copy (&token->location,
                                                                  &str2->location) ;
                                }
                                goto done;
                        }
                } 
                goto fallback;
                break;

        case 'r':
                if (BYTE (input, 2, NULL) == 'g'
                    && BYTE (input, 3, NULL) == 'b'
                    && BYTE (input, 4, NULL) == '(') {
                        status = cr_tknzr_parse_rgb (a_this, &rgb);
                        if (status == CR_OK && rgb) {
                                status = cr_token_set_rgb (token, rgb);
                                CHECK_PARSING_STATUS (status, TRUE);
                                if (rgb) {
                                        cr_parsing_location_copy (&token->location, 
                                                                  &rgb->location) ;
                                }
                                rgb = NULL;
                                goto done;
                        }

                }
                goto fallback;
                break;

        case '<':
                if (BYTE (input, 2, NULL) == '!'
                    && BYTE (input, 3, NULL) == '-'
                    && BYTE (input, 4, NULL) == '-') {
                        SKIP_CHARS (a_this, 1);
                        cr_tknzr_get_parsing_location (a_this, 
                                                       &location) ;
                        SKIP_CHARS (a_this, 3);
                        status = cr_token_set_cdo (token);
                        CHECK_PARSING_STATUS (status, TRUE);
                        cr_parsing_location_copy (&token->location, 
                                                  &location) ;
                        goto done;
                }
                break;

        case '-':
                if (BYTE (input, 2, NULL) == '-'
                    && BYTE (input, 3, NULL) == '>') {
                        SKIP_CHARS (a_this, 1);
                        cr_tknzr_get_parsing_location (a_this, 
                                                       &location) ;
                        SKIP_CHARS (a_this, 2);
                        status = cr_token_set_cdc (token);
                        CHECK_PARSING_STATUS (status, TRUE);
                        cr_parsing_location_copy (&token->location, 
                                                  &location) ;
                        goto done;
                } else {
                        status = cr_tknzr_parse_ident
                                (a_this, &str);
                        if (status == CR_OK) {
                                cr_token_set_ident
                                        (token, str);
                                if (str) {
                                        cr_parsing_location_copy (&token->location, 
                                                                  &str->location) ;
                                }
                                goto done;
                        } else {
                                goto parse_number;
                        }
                }
                break;

        case '~':
                if (BYTE (input, 2, NULL) == '=') {
                        SKIP_CHARS (a_this, 1);
                        cr_tknzr_get_parsing_location (a_this, 
                                                       &location) ;
                        SKIP_CHARS (a_this, 1);
                        status = cr_token_set_includes (token);
                        CHECK_PARSING_STATUS (status, TRUE);
                        cr_parsing_location_copy (&token->location, 
                                                  &location) ;
                        goto done;
                }
                break;

        case '|':
                if (BYTE (input, 2, NULL) == '=') {
                        SKIP_CHARS (a_this, 1);
                        cr_tknzr_get_parsing_location (a_this, 
                                                       &location) ;
                        SKIP_CHARS (a_this, 1);
                        status = cr_token_set_dashmatch (token);
                        CHECK_PARSING_STATUS (status, TRUE);
                        cr_parsing_location_copy (&token->location,
                                                  &location) ;
                        goto done;
                }
                break;

        case '/':
                if (BYTE (input, 2, NULL) == '*') {
                        status = cr_tknzr_parse_comment (a_this, &str);

                        if (status == CR_OK) {
                                status = cr_token_set_comment (token, str);
                                str = NULL;
                                CHECK_PARSING_STATUS (status, TRUE);
                                if (str) {
                                        cr_parsing_location_copy (&token->location, 
                                                                  &str->location) ;
                                }
                                goto done;
                        }
                }
                break ;

        case ';':
                SKIP_CHARS (a_this, 1);
                cr_tknzr_get_parsing_location (a_this, 
                                               &location) ;
                status = cr_token_set_semicolon (token);
                CHECK_PARSING_STATUS (status, TRUE);
                cr_parsing_location_copy (&token->location, 
                                          &location) ;
                goto done;

        case '{':
                SKIP_CHARS (a_this, 1);
                cr_tknzr_get_parsing_location (a_this, 
                                               &location) ;
                status = cr_token_set_cbo (token);
                CHECK_PARSING_STATUS (status, TRUE);
                cr_tknzr_get_parsing_location (a_this, 
                                               &location) ;
                goto done;

        case '}':
                SKIP_CHARS (a_this, 1);
                cr_tknzr_get_parsing_location (a_this, 
                                               &location) ;
                status = cr_token_set_cbc (token);
                CHECK_PARSING_STATUS (status, TRUE);
                cr_parsing_location_copy (&token->location, 
                                          &location) ;
                goto done;

        case '(':
                SKIP_CHARS (a_this, 1);
                cr_tknzr_get_parsing_location (a_this, 
                                               &location) ;
                status = cr_token_set_po (token);
                CHECK_PARSING_STATUS (status, TRUE);
                cr_parsing_location_copy (&token->location, 
                                          &location) ;
                goto done;

        case ')':
                SKIP_CHARS (a_this, 1);
                cr_tknzr_get_parsing_location (a_this, 
                                               &location) ;
                status = cr_token_set_pc (token);
                CHECK_PARSING_STATUS (status, TRUE);
                cr_parsing_location_copy (&token->location, 
                                          &location) ;
                goto done;

        case '[':
                SKIP_CHARS (a_this, 1);
                cr_tknzr_get_parsing_location (a_this, 
                                               &location) ;
                status = cr_token_set_bo (token);
                CHECK_PARSING_STATUS (status, TRUE);
                cr_parsing_location_copy (&token->location, 
                                          &location) ;
                goto done;

        case ']':
                SKIP_CHARS (a_this, 1);
                cr_tknzr_get_parsing_location (a_this, 
                                               &location) ;
                status = cr_token_set_bc (token);
                CHECK_PARSING_STATUS (status, TRUE);
                cr_parsing_location_copy (&token->location, 
                                          &location) ;
                goto done;

        case ' ':
        case '\t':
        case '\n':
        case '\f':
        case '\r':
                {
                        guchar *start = NULL,
                                *end = NULL;

                        status = cr_tknzr_parse_w (a_this, &start, 
                                                   &end, &location);
                        if (status == CR_OK) {
                                status = cr_token_set_s (token);
                                CHECK_PARSING_STATUS (status, TRUE);
                                cr_tknzr_get_parsing_location (a_this, 
                                                               &location) ;
                                goto done;
                        }
                }
                break;

        case '#':
                {
                        status = cr_tknzr_parse_hash (a_this, &str);
                        if (status == CR_OK && str) {
                                status = cr_token_set_hash (token, str);
                                CHECK_PARSING_STATUS (status, TRUE);
                                if (str) {
                                        cr_parsing_location_copy (&token->location,
                                                                  &str->location) ;
                                }
                                str = NULL;
                                goto done;
                        }
                }
                break;

        case '\'':
        case '"':
                status = cr_tknzr_parse_string (a_this, &str);
                if (status == CR_OK && str) {
                        status = cr_token_set_string (token, str);
                        CHECK_PARSING_STATUS (status, TRUE);
                        if (str) {
                                cr_parsing_location_copy (&token->location, 
                                                          &str->location) ;
                        }
                        str = NULL;
                        goto done;
                }
                break;

        case '!':
                status = cr_tknzr_parse_important (a_this, &location);
                if (status == CR_OK) {
                        status = cr_token_set_important_sym (token);
                        CHECK_PARSING_STATUS (status, TRUE);
                        cr_parsing_location_copy (&token->location, 
                                                  &location) ;
                        goto done;
                }
                break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
        case '+':
        /* '-' case is handled separately above for --> comments */
        parse_number:
                {
                        CRNum *num = NULL;

                        status = cr_tknzr_parse_num (a_this, &num);
                        if (status == CR_OK && num) {
                                next_bytes[0] = BYTE (input, 1, NULL);
                                next_bytes[1] = BYTE (input, 2, NULL);
                                next_bytes[2] = BYTE (input, 3, NULL);
                                next_bytes[3] = BYTE (input, 4, NULL);

                                if (next_bytes[0] == 'e'
                                    && next_bytes[1] == 'm') {
                                        num->type = NUM_LENGTH_EM;
                                        status = cr_token_set_ems (token,
                                                                   num);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 2);
                                } else if (next_bytes[0] == 'e'
                                           && next_bytes[1] == 'x') {
                                        num->type = NUM_LENGTH_EX;
                                        status = cr_token_set_exs (token,
                                                                   num);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 2);
                                } else if (next_bytes[0] == 'p'
                                           && next_bytes[1] == 'x') {
                                        num->type = NUM_LENGTH_PX;
                                        status = cr_token_set_length
                                                (token, num, LENGTH_PX_ET);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 2);
                                } else if (next_bytes[0] == 'c'
                                           && next_bytes[1] == 'm') {
                                        num->type = NUM_LENGTH_CM;
                                        status = cr_token_set_length
                                                (token, num, LENGTH_CM_ET);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 2);
                                } else if (next_bytes[0] == 'm'
                                           && next_bytes[1] == 'm') {
                                        num->type = NUM_LENGTH_MM;
                                        status = cr_token_set_length
                                                (token, num, LENGTH_MM_ET);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 2);
                                } else if (next_bytes[0] == 'i'
                                           && next_bytes[1] == 'n') {
                                        num->type = NUM_LENGTH_IN;
                                        status = cr_token_set_length
                                                (token, num, LENGTH_IN_ET);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 2);
                                } else if (next_bytes[0] == 'p'
                                           && next_bytes[1] == 't') {
                                        num->type = NUM_LENGTH_PT;
                                        status = cr_token_set_length
                                                (token, num, LENGTH_PT_ET);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 2);
                                } else if (next_bytes[0] == 'p'
                                           && next_bytes[1] == 'c') {
                                        num->type = NUM_LENGTH_PC;
                                        status = cr_token_set_length
                                                (token, num, LENGTH_PC_ET);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 2);
                                } else if (next_bytes[0] == 'd'
                                           && next_bytes[1] == 'e'
                                           && next_bytes[2] == 'g') {
                                        num->type = NUM_ANGLE_DEG;
                                        status = cr_token_set_angle
                                                (token, num, ANGLE_DEG_ET);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 3);
                                } else if (next_bytes[0] == 'r'
                                           && next_bytes[1] == 'a'
                                           && next_bytes[2] == 'd') {
                                        num->type = NUM_ANGLE_RAD;
                                        status = cr_token_set_angle
                                                (token, num, ANGLE_RAD_ET);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 3);
                                } else if (next_bytes[0] == 'g'
                                           && next_bytes[1] == 'r'
                                           && next_bytes[2] == 'a'
                                           && next_bytes[3] == 'd') {
                                        num->type = NUM_ANGLE_GRAD;
                                        status = cr_token_set_angle
                                                (token, num, ANGLE_GRAD_ET);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 4);
                                } else if (next_bytes[0] == 'm'
                                           && next_bytes[1] == 's') {
                                        num->type = NUM_TIME_MS;
                                        status = cr_token_set_time
                                                (token, num, TIME_MS_ET);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 2);
                                } else if (next_bytes[0] == 's') {
                                        num->type = NUM_TIME_S;
                                        status = cr_token_set_time
                                                (token, num, TIME_S_ET);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 1);
                                } else if (next_bytes[0] == 'H'
                                           && next_bytes[1] == 'z') {
                                        num->type = NUM_FREQ_HZ;
                                        status = cr_token_set_freq
                                                (token, num, FREQ_HZ_ET);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 2);
                                } else if (next_bytes[0] == 'k'
                                           && next_bytes[1] == 'H'
                                           && next_bytes[2] == 'z') {
                                        num->type = NUM_FREQ_KHZ;
                                        status = cr_token_set_freq
                                                (token, num, FREQ_KHZ_ET);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 3);
                                } else if (next_bytes[0] == '%') {
                                        num->type = NUM_PERCENTAGE;
                                        status = cr_token_set_percentage
                                                (token, num);
                                        num = NULL;
                                        SKIP_CHARS (a_this, 1);
                                } else {
                                        status = cr_tknzr_parse_ident (a_this,
                                                                       &str);
                                        if (status == CR_OK && str) {
                                                num->type = NUM_UNKNOWN_TYPE;
                                                status = cr_token_set_dimen
                                                        (token, num, str);
                                                num = NULL;
                                                CHECK_PARSING_STATUS (status,
                                                                      TRUE);
                                                str = NULL;
                                        } else {
                                                status = cr_token_set_number
                                                        (token, num);
                                                num = NULL;
                                                CHECK_PARSING_STATUS (status, CR_OK);
                                                str = NULL;
                                        }
                                }
                                if (token && token->u.num) {
                                        cr_parsing_location_copy (&token->location,
                                                                  &token->u.num->location) ;
                                } else {
                                        status = CR_ERROR ;
                                }
                                goto done ;
                        }
                }
                break;

        default:
        fallback:
                /*process the fallback cases here */

                if (next_char == '\\'
                    || (cr_utils_is_nonascii (next_bytes[0]) == TRUE)
                    || ((next_char >= 'a') && (next_char <= 'z'))
                    || ((next_char >= 'A') && (next_char <= 'Z'))) {
                        status = cr_tknzr_parse_ident (a_this, &str);
                        if (status == CR_OK && str) {
                                guint32 next_c = 0;

                                status = cr_input_peek_char
                                        (PRIVATE (a_this)->input, &next_c);

                                if (status == CR_OK && next_c == '(') {

                                        SKIP_CHARS (a_this, 1);
                                        status = cr_token_set_function
                                                (token, str);
                                        CHECK_PARSING_STATUS (status, TRUE);
                                        /*ownership is transfered
                                         *to token by cr_token_set_function.
                                         */
                                        if (str) {
                                                cr_parsing_location_copy (&token->location, 
                                                                          &str->location) ;
                                        }
                                        str = NULL;
                                } else {
                                        status = cr_token_set_ident (token,
                                                                     str);
                                        CHECK_PARSING_STATUS (status, TRUE);
                                        if (str) {
                                                cr_parsing_location_copy (&token->location, 
                                                                          &str->location) ;
                                        }
                                        str = NULL;
                                }
                                goto done;
                        } else {
                                if (str) {
                                        cr_string_destroy (str);
                                        str = NULL;
                                }
                        }
                }
                break;
        }

        READ_NEXT_CHAR (a_this, &next_char);
        cr_tknzr_get_parsing_location (a_this, 
                                       &location) ;
        status = cr_token_set_delim (token, next_char);
        CHECK_PARSING_STATUS (status, TRUE);
        cr_parsing_location_copy (&token->location, 
                                  &location) ;
 done:

        if (status == CR_OK && token) {
                *a_tk = token;
                /*
                 *store the previous position input stream pos.
                 */
                memmove (&PRIVATE (a_this)->prev_pos,
                         &init_pos, sizeof (CRInputPos));
                return CR_OK;
        }

 error:
        if (token) {
                cr_token_destroy (token);
                token = NULL;
        }

        if (str) {
                cr_string_destroy (str);
                str = NULL;
        }
        cr_tknzr_set_cur_pos (a_this, &init_pos);
        return status;

}