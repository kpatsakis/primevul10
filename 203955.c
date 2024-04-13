cr_input_set_cur_index (CRInput * a_this, glong a_index)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this), CR_BAD_PARAM_ERROR);

        PRIVATE (a_this)->next_byte_index = a_index;

        return CR_OK;
}