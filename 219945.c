u_get_headentry(void)
{
    if (curbuf->b_u_newhead == NULL || curbuf->b_u_newhead->uh_entry == NULL)
    {
	IEMSG(_("E439: undo list corrupt"));
	return NULL;
    }
    return curbuf->b_u_newhead->uh_entry;
}