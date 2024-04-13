cr_tknzr_get_parsing_location (CRTknzr *a_this,
                               CRParsingLocation *a_loc)
{
        g_return_val_if_fail (a_this 
                              && PRIVATE (a_this)
                              && a_loc,
                              CR_BAD_PARAM_ERROR) ;

        return cr_input_get_parsing_location 
                (PRIVATE (a_this)->input, a_loc) ;
}