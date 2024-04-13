cr_input_get_line_num (CRInput const * a_this, glong * a_line_num)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && a_line_num, CR_BAD_PARAM_ERROR);

        *a_line_num = PRIVATE (a_this)->line;

        return CR_OK;
}