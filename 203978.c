cr_input_increment_line_num (CRInput * a_this, glong a_increment)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this), CR_BAD_PARAM_ERROR);

        PRIVATE (a_this)->line += a_increment;

        return CR_OK;
}