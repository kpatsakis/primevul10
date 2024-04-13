cr_input_set_column_num (CRInput * a_this, glong a_col)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this), CR_BAD_PARAM_ERROR);

        PRIVATE (a_this)->col = a_col;

        return CR_OK;
}