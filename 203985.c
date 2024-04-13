cr_input_get_column_num (CRInput const * a_this, glong * a_col)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this) && a_col,
                              CR_BAD_PARAM_ERROR);

        *a_col = PRIVATE (a_this)->col;

        return CR_OK;
}