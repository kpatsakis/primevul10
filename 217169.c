set_ref_in_insexpand_funcs(int copyID)
{
    int abort = FALSE;

    abort = set_ref_in_callback(&cfu_cb, copyID);
    abort = abort || set_ref_in_callback(&ofu_cb, copyID);
    abort = abort || set_ref_in_callback(&tsrfu_cb, copyID);

    return abort;
}