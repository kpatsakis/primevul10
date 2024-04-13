cr_input_get_byte_addr (CRInput * a_this, gulong a_offset)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this), NULL);

        if (a_offset >= PRIVATE (a_this)->nb_bytes) {
                return NULL;
        }

        return &PRIVATE (a_this)->in_buf[a_offset];
}