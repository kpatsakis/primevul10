cr_input_peek_byte (CRInput const * a_this, enum CRSeekPos a_origin,
                    gulong a_offset, guchar * a_byte)
{
        gulong abs_offset = 0;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && a_byte, CR_BAD_PARAM_ERROR);

        switch (a_origin) {

        case CR_SEEK_CUR:
                abs_offset = PRIVATE (a_this)->next_byte_index - 1 + a_offset;
                break;

        case CR_SEEK_BEGIN:
                abs_offset = a_offset;
                break;

        case CR_SEEK_END:
                abs_offset = PRIVATE (a_this)->in_buf_size - 1 - a_offset;
                break;

        default:
                return CR_BAD_PARAM_ERROR;
        }

        if (abs_offset < PRIVATE (a_this)->in_buf_size) {

                *a_byte = PRIVATE (a_this)->in_buf[abs_offset];

                return CR_OK;

        } else {
                return CR_END_OF_INPUT_ERROR;
        }
}