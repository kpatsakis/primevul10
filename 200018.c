isdn_capi_rec_hl_msg(capi_msg *cm) {
	
	int di;
	int ch;
	
	di = (cm->adr.Controller & 0x7f) -1;
	ch = isdn_dc2minor(di, (cm->adr.Controller>>8)& 0x7f);
	switch(cm->Command) {
		case CAPI_FACILITY:
			/* in the moment only handled in tty */
			return(isdn_tty_capi_facility(cm));
		default:
			return(-1);
	}
}