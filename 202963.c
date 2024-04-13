rfbClientListInit(rfbScreenInfoPtr rfbScreen)
{
    if(sizeof(rfbBool)!=1) {
        /* a sanity check */
        fprintf(stderr,"rfbBool's size is not 1 (%lu)!\n",(unsigned long)sizeof(rfbBool));
	/* we cannot continue, because rfbBool is supposed to be char everywhere */
	exit(1);
    }
    rfbScreen->rfbClientHead = NULL;
}