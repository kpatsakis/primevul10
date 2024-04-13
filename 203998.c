cr_tknzr_ref (CRTknzr * a_this)
{
        g_return_if_fail (a_this && PRIVATE (a_this));

        PRIVATE (a_this)->ref_count++;
}