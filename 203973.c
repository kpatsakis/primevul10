cr_input_unref (CRInput * a_this)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this), FALSE);

        if (PRIVATE (a_this)->ref_count) {
                PRIVATE (a_this)->ref_count--;
        }

        if (PRIVATE (a_this)->ref_count == 0) {
                cr_input_destroy (a_this);
                return TRUE;
        }
        return FALSE;
}