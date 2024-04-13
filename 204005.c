cr_tknzr_peek_byte2 (CRTknzr * a_this, gulong a_offset, gboolean * a_eof)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input, 0);

        return cr_input_peek_byte2 (PRIVATE (a_this)->input, a_offset, a_eof);
}