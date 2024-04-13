cr_tknzr_peek_byte (CRTknzr * a_this, gulong a_offset, guchar * a_byte)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input && a_byte,
                              CR_BAD_PARAM_ERROR);

        if (PRIVATE (a_this)->token_cache) {
                cr_input_set_cur_pos (PRIVATE (a_this)->input,
                                      &PRIVATE (a_this)->prev_pos);
                cr_token_destroy (PRIVATE (a_this)->token_cache);
                PRIVATE (a_this)->token_cache = NULL;
        }

        return cr_input_peek_byte (PRIVATE (a_this)->input,
                                   CR_SEEK_CUR, a_offset, a_byte);
}