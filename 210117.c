static void jsiPop(Jsi_Interp* interp, int n) {
    int t = n;
    while (t > 0) {
        Assert((interp->framePtr->Sp-t)>=0);
/*        Jsi_Value *v = _jsi_STACKIDX(interp->framePtr->Sp-t);
         if (v->refCnt>1) puts("OO");*/
        jsiClearStack(interp,t);
        --t;
    }
    interp->framePtr->Sp -= n;
}