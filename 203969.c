cr_input_get_end_of_file (CRInput const * a_this, gboolean * a_eof)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && a_eof, CR_BAD_PARAM_ERROR);

        *a_eof = PRIVATE (a_this)->end_of_input;

        return CR_OK;
}