static void SetupStdChan(Jsi_Chan* ch, FILE *fp, Jsi_Filesystem *fsPtr, int flags) {
    memset(ch, 0, sizeof(*ch));
    ch->fp = fp;
    ch->fsPtr = fsPtr;
    ch->flags = flags|JSI_FS_NOCLOSE;
}