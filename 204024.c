cr_tknzr_parse_name (CRTknzr * a_this, 
                     CRString ** a_str)
{
        guint32 tmp_char = 0;
        CRInputPos init_pos;
        enum CRStatus status = CR_OK;
        gboolean str_needs_free = FALSE,
                is_first_nmchar=TRUE ;
        glong i = 0;
        CRParsingLocation loc = {0} ;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input
                              && a_str,
                              CR_BAD_PARAM_ERROR) ;

        RECORD_INITIAL_POS (a_this, &init_pos);

        if (*a_str == NULL) {
                *a_str = cr_string_new ();
                str_needs_free = TRUE;
        }
        for (i = 0;; i++) {
                if (is_first_nmchar == TRUE) {
                        status = cr_tknzr_parse_nmchar 
                                (a_this, &tmp_char,
                                 &loc) ;
                        is_first_nmchar = FALSE ;
                } else {
                        status = cr_tknzr_parse_nmchar 
                                (a_this, &tmp_char, NULL) ;
                }
                if (status != CR_OK)
                        break;                
                g_string_append_unichar ((*a_str)->stryng, 
                                         tmp_char);
        }
        if (i > 0) {
                cr_parsing_location_copy 
                        (&(*a_str)->location, &loc) ;
                return CR_OK;
        }
        if (str_needs_free == TRUE && *a_str) {
                cr_string_destroy (*a_str);
                *a_str = NULL;
        }
        cr_tknzr_set_cur_pos (a_this, &init_pos);
        return CR_PARSING_ERROR;
}