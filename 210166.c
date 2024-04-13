Jsi_Interp *jsi_DoExit(Jsi_Interp *interp, int rc)
{
    if (rc<0 || rc>127) rc = 127;
    if (!interp || !interp->opts.no_exit) {
        if (rc) {
            Jsi_Flush(interp, jsi_Stdout);
            Jsi_Flush(interp, jsi_Stderr);
        }
        exit(rc);
    }
    fprintf(stderr, "ignoring attempted exit: may cause a crash\n");
    if (interp) interp->deleting = 1;
    return NULL;
}