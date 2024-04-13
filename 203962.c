cr_input_consume_chars (CRInput * a_this, guint32 a_char, gulong * a_nb_char)
{
        enum CRStatus status = CR_OK;
        gulong nb_consumed = 0;

        g_return_val_if_fail (a_this && PRIVATE (a_this) && a_nb_char,
                              CR_BAD_PARAM_ERROR);

        g_return_val_if_fail (a_char != 0 || a_nb_char != NULL,
                              CR_BAD_PARAM_ERROR);

        for (nb_consumed = 0; ((status == CR_OK)
                               && (*a_nb_char > 0
                                   && nb_consumed < *a_nb_char));
             nb_consumed++) {
                status = cr_input_consume_char (a_this, a_char);
        }

        *a_nb_char = nb_consumed;

        if ((nb_consumed > 0)
            && ((status == CR_PARSING_ERROR)
                || (status == CR_END_OF_INPUT_ERROR))) {
                status = CR_OK;
        }

        return status;
}