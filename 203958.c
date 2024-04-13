cr_input_peek_byte2 (CRInput const * a_this, gulong a_offset, gboolean * a_eof)
{
        guchar result = 0;
        enum CRStatus status = CR_ERROR;

        g_return_val_if_fail (a_this && PRIVATE (a_this), 0);

        if (a_eof)
                *a_eof = FALSE;

        status = cr_input_peek_byte (a_this, CR_SEEK_CUR, a_offset, &result);

        if ((status == CR_END_OF_INPUT_ERROR)
            && a_eof)
                *a_eof = TRUE;

        return result;
}