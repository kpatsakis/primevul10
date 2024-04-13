cr_input_seek_index (CRInput * a_this, enum CRSeekPos a_origin, gint a_pos)
{

        glong abs_offset = 0;

        g_return_val_if_fail (a_this && PRIVATE (a_this), CR_BAD_PARAM_ERROR);

        switch (a_origin) {

        case CR_SEEK_CUR:
                abs_offset = PRIVATE (a_this)->next_byte_index - 1 + a_pos;
                break;

        case CR_SEEK_BEGIN:
                abs_offset = a_pos;
                break;

        case CR_SEEK_END:
                abs_offset = PRIVATE (a_this)->in_buf_size - 1 - a_pos;
                break;

        default:
                return CR_BAD_PARAM_ERROR;
        }

        if ((abs_offset > 0)
            && (gulong) abs_offset < PRIVATE (a_this)->nb_bytes) {

                /*update the input stream's internal state */
                PRIVATE (a_this)->next_byte_index = abs_offset + 1;

                return CR_OK;
        }

        return CR_OUT_OF_BOUNDS_ERROR;
}