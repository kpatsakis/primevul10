free_insexpand_stuff(void)
{
    VIM_CLEAR(compl_orig_text);
# ifdef FEAT_EVAL
    free_callback(&cfu_cb);
    free_callback(&ofu_cb);
    free_callback(&tsrfu_cb);
# endif
}