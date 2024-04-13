cr_tknzr_destroy (CRTknzr * a_this)
{
        g_return_if_fail (a_this);

        if (PRIVATE (a_this) && PRIVATE (a_this)->input) {
                if (cr_input_unref (PRIVATE (a_this)->input)
                    == TRUE) {
                        PRIVATE (a_this)->input = NULL;
                }
        }

        if (PRIVATE (a_this)->token_cache) {
                cr_token_destroy (PRIVATE (a_this)->token_cache);
                PRIVATE (a_this)->token_cache = NULL;
        }

        if (PRIVATE (a_this)) {
                g_free (PRIVATE (a_this));
                PRIVATE (a_this) = NULL;
        }

        g_free (a_this);
}