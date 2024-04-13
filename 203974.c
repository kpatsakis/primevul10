cr_input_get_end_of_line (CRInput const * a_this, gboolean * a_eol)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && a_eol, CR_BAD_PARAM_ERROR);

        *a_eol = PRIVATE (a_this)->end_of_line;

        return CR_OK;
}