cr_input_consume_white_spaces (CRInput * a_this, gulong * a_nb_chars)
{
        enum CRStatus status = CR_OK;
        guint32 cur_char = 0,
                nb_consumed = 0;

        g_return_val_if_fail (a_this && PRIVATE (a_this) && a_nb_chars,
                              CR_BAD_PARAM_ERROR);

        for (nb_consumed = 0;
             ((*a_nb_chars > 0) && (nb_consumed < *a_nb_chars));
             nb_consumed++) {
                status = cr_input_peek_char (a_this, &cur_char);
                if (status != CR_OK)
                        break;

                /*if the next char is a white space, consume it ! */
                if (cr_utils_is_white_space (cur_char) == TRUE) {
                        status = cr_input_read_char (a_this, &cur_char);
                        if (status != CR_OK)
                                break;
                        continue;
                }

                break;

        }

	*a_nb_chars = (gulong) nb_consumed;

        if (nb_consumed && status == CR_END_OF_INPUT_ERROR) {
                status = CR_OK;
        }

        return status;
}