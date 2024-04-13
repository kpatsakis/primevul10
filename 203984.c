cr_input_destroy (CRInput * a_this)
{
        if (a_this == NULL)
                return;

        if (PRIVATE (a_this)) {
                if (PRIVATE (a_this)->in_buf && PRIVATE (a_this)->free_in_buf) {
                        g_free (PRIVATE (a_this)->in_buf);
                        PRIVATE (a_this)->in_buf = NULL;
                }

                g_free (PRIVATE (a_this));
                PRIVATE (a_this) = NULL;
        }

        g_free (a_this);
}