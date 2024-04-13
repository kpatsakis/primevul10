cr_tknzr_get_cur_byte_addr (CRTknzr * a_this, guchar ** a_addr)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input, CR_BAD_PARAM_ERROR);
        if (PRIVATE (a_this)->token_cache) {
                cr_input_set_cur_pos (PRIVATE (a_this)->input,
                                      &PRIVATE (a_this)->prev_pos);
                cr_token_destroy (PRIVATE (a_this)->token_cache);
                PRIVATE (a_this)->token_cache = NULL;
        }

        return cr_input_get_cur_byte_addr (PRIVATE (a_this)->input, a_addr);
}