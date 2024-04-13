cr_tknzr_consume_chars (CRTknzr * a_this, guint32 a_char, glong * a_nb_char)
{
	gulong consumed = *(gulong *) a_nb_char;
	enum CRStatus status;
        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input, CR_BAD_PARAM_ERROR);

        if (PRIVATE (a_this)->token_cache) {
                cr_input_set_cur_pos (PRIVATE (a_this)->input,
                                      &PRIVATE (a_this)->prev_pos);
                cr_token_destroy (PRIVATE (a_this)->token_cache);
                PRIVATE (a_this)->token_cache = NULL;
        }

        status = cr_input_consume_chars (PRIVATE (a_this)->input,
                                         a_char, &consumed);
	*a_nb_char = (glong) consumed;
	return status;
}