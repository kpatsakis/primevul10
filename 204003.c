cr_tknzr_set_cur_pos (CRTknzr * a_this, CRInputPos * a_pos)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input, CR_BAD_PARAM_ERROR);

        if (PRIVATE (a_this)->token_cache) {
                cr_token_destroy (PRIVATE (a_this)->token_cache);
                PRIVATE (a_this)->token_cache = NULL;
        }

        return cr_input_set_cur_pos (PRIVATE (a_this)->input, a_pos);
}