cr_input_set_end_of_line (CRInput * a_this, gboolean a_eol)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this), CR_BAD_PARAM_ERROR);

        PRIVATE (a_this)->end_of_line = a_eol;

        return CR_OK;
}