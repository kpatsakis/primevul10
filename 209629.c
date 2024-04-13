static Jsi_RC freeEventTbl(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *ptr) {
    Jsi_Event *event = (Jsi_Event *)ptr;
    SIGASSERT(event,EVENT);
    if (!ptr) return JSI_OK;
    Jsi_HashValueSet(event->hPtr, NULL);
    event->hPtr = NULL;
    Jsi_EventFree(interp, event);
    return JSI_OK;
}