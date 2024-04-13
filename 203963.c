cr_input_get_cur_byte_addr (CRInput * a_this, guchar ** a_offset)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this) && a_offset,
                              CR_BAD_PARAM_ERROR);

        if (!PRIVATE (a_this)->next_byte_index) {
                return CR_START_OF_INPUT_ERROR;
        }

        *a_offset = cr_input_get_byte_addr
                (a_this, PRIVATE (a_this)->next_byte_index - 1);

        return CR_OK;
}