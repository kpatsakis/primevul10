cr_input_consume_char (CRInput * a_this, guint32 a_char)
{
        guint32 c;
        enum CRStatus status;

        g_return_val_if_fail (a_this && PRIVATE (a_this), CR_BAD_PARAM_ERROR);

        if ((status = cr_input_peek_char (a_this, &c)) != CR_OK) {
                return status;
        }

        if (c == a_char || a_char == 0) {
                status = cr_input_read_char (a_this, &c);
        } else {
                return CR_PARSING_ERROR;
        }

        return status;
}