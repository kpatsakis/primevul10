cr_input_get_cur_index (CRInput const * a_this, glong * a_index)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && a_index, CR_BAD_PARAM_ERROR);

        *a_index = PRIVATE (a_this)->next_byte_index;

        return CR_OK;
}