cr_tknzr_read_byte (CRTknzr * a_this, guchar * a_byte)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this), CR_BAD_PARAM_ERROR);

        return cr_input_read_byte (PRIVATE (a_this)->input, a_byte);

}