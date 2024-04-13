cr_tknzr_set_input (CRTknzr * a_this, CRInput * a_input)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this), CR_BAD_PARAM_ERROR);

        if (PRIVATE (a_this)->input) {
                cr_input_unref (PRIVATE (a_this)->input);
        }

        PRIVATE (a_this)->input = a_input;

        cr_input_ref (PRIVATE (a_this)->input);

        return CR_OK;
}