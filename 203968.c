cr_input_increment_col_num (CRInput * a_this, glong a_increment)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this), CR_BAD_PARAM_ERROR);

        PRIVATE (a_this)->col += a_increment;

        return CR_OK;
}