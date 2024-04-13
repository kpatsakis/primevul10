cr_input_get_nb_bytes_left (CRInput const * a_this)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this), -1);
        g_return_val_if_fail (PRIVATE (a_this)->nb_bytes
                              <= PRIVATE (a_this)->in_buf_size, -1);
        g_return_val_if_fail (PRIVATE (a_this)->next_byte_index
                              <= PRIVATE (a_this)->nb_bytes, -1);

        if (PRIVATE (a_this)->end_of_input)
                return 0;

        return PRIVATE (a_this)->nb_bytes - PRIVATE (a_this)->next_byte_index;
}