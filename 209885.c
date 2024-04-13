static void jsiVarDeref(Jsi_Interp* interp, int n) {
    while(interp->framePtr->Sp<n) // Assert and Log may map-out Ops.
        jsiPush(interp, 1);
    int i;
    for (i=1; i<=n; i++) {
        Jsi_Value *vb = _jsi_STACKIDX(interp->framePtr->Sp - i);
        if (vb->vt == JSI_VT_VARIABLE) {
            SIGASSERTV(vb->d.lval, VALUE);
            Jsi_ValueCopy(interp, vb, vb->d.lval);
        }
    }
}