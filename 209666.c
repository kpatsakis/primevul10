static void DeleteAllInterps() { /* Delete toplevel interps. */
    Jsi_HashEntry *hPtr;
    Jsi_HashSearch search;
    if (!jsiIntData.interpsTbl)
        return;
    for (hPtr = Jsi_HashSearchFirst(jsiIntData.interpsTbl, &search); hPtr; hPtr = Jsi_HashSearchNext(&search)) {
        Jsi_Interp *interp = (Jsi_Interp *)Jsi_HashKeyGet(hPtr);
        Jsi_HashEntryDelete(hPtr);
        interp->destroying = 1;
        Jsi_InterpDelete(interp);
    }
    Jsi_HashDelete(jsiIntData.interpsTbl);
    jsiIntData.interpsTbl = NULL;
    jsiIntData.isInit = 0;
}