cr_tknzr_try_to_skip_spaces (CRTknzr * a_this)
{
        enum CRStatus status = CR_ERROR;
        guint32 cur_char = 0;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input, CR_BAD_PARAM_ERROR);

        status = cr_input_peek_char (PRIVATE (a_this)->input, &cur_char);

        if (status != CR_OK) {
                if (status == CR_END_OF_INPUT_ERROR)
                        return CR_OK;
                return status;
        }

        if (cr_utils_is_white_space (cur_char) == TRUE) {
                gulong nb_chars = -1; /*consume all spaces */

                status = cr_input_consume_white_spaces
                        (PRIVATE (a_this)->input, &nb_chars);
        }

        return status;
}