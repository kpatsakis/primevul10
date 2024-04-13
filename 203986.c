cr_input_end_of_input (CRInput const * a_this, gboolean * a_end_of_input)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && a_end_of_input, CR_BAD_PARAM_ERROR);

        *a_end_of_input = (PRIVATE (a_this)->next_byte_index
                           >= PRIVATE (a_this)->in_buf_size) ? TRUE : FALSE;

        return CR_OK;
}