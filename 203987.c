cr_input_set_line_num (CRInput * a_this, glong a_line_num)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this), CR_BAD_PARAM_ERROR);

        PRIVATE (a_this)->line = a_line_num;

        return CR_OK;
}