static void jsiPush(Jsi_Interp* interp, int n) {
    int i = 0;
    do {
        if (!_jsi_STACKIDX(interp->framePtr->Sp))
            _jsi_STACKIDX(interp->framePtr->Sp) = Jsi_ValueNew1(interp);
        if (!_jsi_THISIDX(interp->framePtr->Sp))
            _jsi_THISIDX(interp->framePtr->Sp) = Jsi_ValueNew1(interp);
        if (i++ >= n) break;
        interp->framePtr->Sp++;
    } while (1);
}