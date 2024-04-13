ex_redrawstatus(exarg_T *eap UNUSED)
{
    int		r = RedrawingDisabled;
    int		p = p_lz;

    RedrawingDisabled = 0;
    p_lz = FALSE;
    if (eap->forceit)
	status_redraw_all();
    else
	status_redraw_curbuf();
    update_screen(VIsual_active ? UPD_INVERTED : 0);
    RedrawingDisabled = r;
    p_lz = p;
    out_flush();
}