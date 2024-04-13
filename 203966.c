cr_input_set_end_of_file (CRInput * a_this, gboolean a_eof)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this), CR_BAD_PARAM_ERROR);

        PRIVATE (a_this)->end_of_input = a_eof;

        return CR_OK;
}