rfbClientListInit(rfbScreenInfoPtr rfbScreen)
{
    if(sizeof(rfbBool)!=1) {
        /* a sanity check */
        fprintf(stderr,"rfbBool's size is not 1 (%d)!\n",(int)sizeof(rfbBool));
	/* we cannot continue, because rfbBool is supposed to be char everywhere */
	exit(1);
    }
    rfbScreen->clientHead = NULL;
    INIT_MUTEX(rfbClientListMutex);
}