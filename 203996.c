cr_tknzr_seek_index (CRTknzr * a_this, enum CRSeekPos a_origin, gint a_pos)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input, CR_BAD_PARAM_ERROR);

        if (PRIVATE (a_this)->token_cache) {
                cr_input_set_cur_pos (PRIVATE (a_this)->input,
                                      &PRIVATE (a_this)->prev_pos);
                cr_token_destroy (PRIVATE (a_this)->token_cache);
                PRIVATE (a_this)->token_cache = NULL;
        }

        return cr_input_seek_index (PRIVATE (a_this)->input, a_origin, a_pos);
}