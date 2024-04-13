cr_tknzr_parse_ident (CRTknzr * a_this, CRString ** a_str)
{
        guint32 tmp_char = 0;
        CRString *stringue = NULL ;
        CRInputPos init_pos;
        enum CRStatus status = CR_OK;
        gboolean location_is_set = FALSE ;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input
                              && a_str, CR_BAD_PARAM_ERROR);

        RECORD_INITIAL_POS (a_this, &init_pos);
        PEEK_NEXT_CHAR (a_this, &tmp_char) ;
        stringue = cr_string_new () ;
        g_return_val_if_fail (stringue, 
                              CR_OUT_OF_MEMORY_ERROR) ;

        if (tmp_char == '-') {
                READ_NEXT_CHAR (a_this, &tmp_char) ;
                cr_tknzr_get_parsing_location
                        (a_this, &stringue->location) ;
                location_is_set = TRUE ;
                g_string_append_unichar (stringue->stryng, 
                                         tmp_char) ;
        }
        status = cr_tknzr_parse_nmstart (a_this, &tmp_char, NULL);
        if (status != CR_OK) {
                status = CR_PARSING_ERROR;
                goto end ;
        }
        if (location_is_set == FALSE) {
                cr_tknzr_get_parsing_location 
                        (a_this, &stringue->location) ;
                location_is_set = TRUE ;
        }
        g_string_append_unichar (stringue->stryng, tmp_char);
        for (;;) {
                status = cr_tknzr_parse_nmchar (a_this, 
                                                &tmp_char, 
                                                NULL);
                if (status != CR_OK) {
                        status = CR_OK ;
                        break;
                }
                g_string_append_unichar (stringue->stryng, tmp_char);
        }
        if (status == CR_OK) {
                if (!*a_str) {
                        *a_str = stringue ;
                
                } else {
                        g_string_append_len ((*a_str)->stryng, 
                                             stringue->stryng->str, 
                                             stringue->stryng->len) ;
                        cr_string_destroy (stringue) ;
                }
                stringue = NULL ;
        }

 error:
 end:
        if (stringue) {
                cr_string_destroy (stringue) ;
                stringue = NULL ;
        }
        if (status != CR_OK ) {
                cr_tknzr_set_cur_pos (a_this, &init_pos) ;
        }
        return status ;
}