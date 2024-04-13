cr_input_get_parsing_location (CRInput const *a_this,
                               CRParsingLocation *a_loc)
{
        g_return_val_if_fail (a_this 
                              && PRIVATE (a_this)
                              && a_loc, 
                              CR_BAD_PARAM_ERROR) ;

        a_loc->line = PRIVATE (a_this)->line ;
        a_loc->column = PRIVATE (a_this)->col ;
        if (PRIVATE (a_this)->next_byte_index) {
                a_loc->byte_offset = PRIVATE (a_this)->next_byte_index - 1 ;
        } else {
                a_loc->byte_offset = PRIVATE (a_this)->next_byte_index  ;
        }
        return CR_OK ;
}