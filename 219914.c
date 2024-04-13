u_freeentry(u_entry_T *uep, long n)
{
    while (n > 0)
	vim_free(uep->ue_array[--n]);
    vim_free((char_u *)uep->ue_array);
#ifdef U_DEBUG
    uep->ue_magic = 0;
#endif
    vim_free((char_u *)uep);
}