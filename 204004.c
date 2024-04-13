cr_tknzr_parse_num (CRTknzr * a_this, 
                    CRNum ** a_num)
{
        enum CRStatus status = CR_PARSING_ERROR;
        enum CRNumType val_type = NUM_GENERIC;
        gboolean parsing_dec,  /* true iff seen decimal point. */
                parsed; /* true iff the substring seen so far is a valid CSS
                           number, i.e. `[0-9]+|[0-9]*\.[0-9]+'. */
        guint32 cur_char = 0,
                next_char = 0;
        gdouble numerator, denominator = 1;
        CRInputPos init_pos;
        CRParsingLocation location = {0} ;
        int sign = 1;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input, 
                              CR_BAD_PARAM_ERROR);

        RECORD_INITIAL_POS (a_this, &init_pos);
        READ_NEXT_CHAR (a_this, &cur_char);

        if (cur_char == '+' || cur_char == '-') {
                if (cur_char == '-') {
                        sign = -1;
                }
                READ_NEXT_CHAR (a_this, &cur_char);
        }

        if (IS_NUM (cur_char)) {
                numerator = (cur_char - '0');
                parsing_dec = FALSE;
                parsed = TRUE;
        } else if (cur_char == '.') {
                numerator = 0;
                parsing_dec = TRUE;
                parsed = FALSE;
        } else {
                status = CR_PARSING_ERROR;
                goto error;
        }
        cr_tknzr_get_parsing_location (a_this, &location) ;

        for (;;) {
                status = cr_tknzr_peek_char (a_this, &next_char);
                if (status != CR_OK) {
                        if (status == CR_END_OF_INPUT_ERROR)
                                status = CR_OK;
                        break;
                }
                if (next_char == '.') {
                        if (parsing_dec) {
                                status = CR_PARSING_ERROR;
                                goto error;
                        }

                        READ_NEXT_CHAR (a_this, &cur_char);
                        parsing_dec = TRUE;
                        parsed = FALSE;  /* In CSS, there must be at least
                                            one digit after `.'. */
                } else if (IS_NUM (next_char)) {
                        READ_NEXT_CHAR (a_this, &cur_char);
                        parsed = TRUE;

                        numerator = numerator * 10 + (cur_char - '0');
                        if (parsing_dec) {
                                denominator *= 10;
                        }
                } else {
                        break;
                }
        }

        if (!parsed) {
                status = CR_PARSING_ERROR;
        }

        /*
         *Now, set the output param values.
         */
        if (status == CR_OK) {
                gdouble val = (numerator / denominator) * sign;
                if (*a_num == NULL) {
                        *a_num = cr_num_new_with_val (val, val_type);

                        if (*a_num == NULL) {
                                status = CR_ERROR;
                                goto error;
                        }
                } else {
                        (*a_num)->val = val;
                        (*a_num)->type = val_type;
                }
                cr_parsing_location_copy (&(*a_num)->location,
                                          &location) ;
                return CR_OK;
        }

 error:

        cr_tknzr_set_cur_pos (a_this, &init_pos);

        return status;
}