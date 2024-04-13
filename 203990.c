cr_tknzr_unget_token (CRTknzr * a_this, CRToken * a_token)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->token_cache == NULL,
                              CR_BAD_PARAM_ERROR);

        PRIVATE (a_this)->token_cache = a_token;

        return CR_OK;
}