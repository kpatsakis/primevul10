cr_tknzr_get_input (CRTknzr * a_this, CRInput ** a_input)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this), CR_BAD_PARAM_ERROR);

        *a_input = PRIVATE (a_this)->input;

        return CR_OK;
}