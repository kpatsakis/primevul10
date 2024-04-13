cr_tknzr_parse_comment (CRTknzr * a_this, 
                        CRString ** a_comment)
{
        enum CRStatus status = CR_OK;
        CRInputPos init_pos;
        guint32 cur_char = 0, next_char= 0;
        CRString *comment = NULL;
        CRParsingLocation loc = {0} ;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input, 
                              CR_BAD_PARAM_ERROR);

        RECORD_INITIAL_POS (a_this, &init_pos);        
        READ_NEXT_CHAR (a_this, &cur_char) ;        
        ENSURE_PARSING_COND (cur_char == '/');
        cr_tknzr_get_parsing_location (a_this, &loc) ;

        READ_NEXT_CHAR (a_this, &cur_char);
        ENSURE_PARSING_COND (cur_char == '*');
        comment = cr_string_new ();
        for (;;) { /* [^*]* */
                PEEK_NEXT_CHAR (a_this, &next_char);
                if (next_char == '*')
                        break;
                READ_NEXT_CHAR (a_this, &cur_char);
                g_string_append_unichar (comment->stryng, cur_char);
        }
        /* Stop condition: next_char == '*' */
        for (;;) { /* \*+ */
                READ_NEXT_CHAR(a_this, &cur_char);
                ENSURE_PARSING_COND (cur_char == '*');
                g_string_append_unichar (comment->stryng, cur_char);
                PEEK_NEXT_CHAR (a_this, &next_char);
                if (next_char != '*')
                        break;
        }
        /* Stop condition: next_char != '*' */
        for (;;) { /* ([^/][^*]*\*+)* */
                if (next_char == '/')
                        break;
                READ_NEXT_CHAR(a_this, &cur_char);
                g_string_append_unichar (comment->stryng, cur_char);
                for (;;) { /* [^*]* */
                        PEEK_NEXT_CHAR (a_this, &next_char);
                        if (next_char == '*')
                                break;
                        READ_NEXT_CHAR (a_this, &cur_char);
                        g_string_append_unichar (comment->stryng, cur_char);
                }
                /* Stop condition: next_char = '*', no need to verify, because peek and read exit to error anyway */
                for (;;) { /* \*+ */
                        READ_NEXT_CHAR(a_this, &cur_char);
                        ENSURE_PARSING_COND (cur_char == '*');
                        g_string_append_unichar (comment->stryng, cur_char);
                        PEEK_NEXT_CHAR (a_this, &next_char);
                        if (next_char != '*')
                                break;
                }
                /* Continue condition: next_char != '*' */
        }
        /* Stop condition: next_char == '\/' */
        READ_NEXT_CHAR(a_this, &cur_char);
        g_string_append_unichar (comment->stryng, cur_char);

        if (status == CR_OK) {
                cr_parsing_location_copy (&comment->location, 
                                          &loc) ;
                *a_comment = comment;                
                return CR_OK;
        }
 error:

        if (comment) {
                cr_string_destroy (comment);
                comment = NULL;
        }

        cr_tknzr_set_cur_pos (a_this, &init_pos);

        return status;
}