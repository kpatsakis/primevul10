void jsi_AddEventHandler(Jsi_Interp *interp)
{
    Jsi_Event *ev;
    while (!interp->EventHdlId) { /* Find an empty event slot. */
        bool isNew;
        uintptr_t id = ++interp->eventIdx;
        Jsi_HashEntry *hPtr = Jsi_HashEntryNew(interp->eventTbl, (void*)id, &isNew);
        if (!isNew)
            continue;
        ev = (Jsi_Event*)Jsi_Calloc(1, sizeof(*ev));
        SIGINIT(ev,EVENT);
        ev->id = id;
        ev->handler = ThreadEvalCallback;
        ev->hPtr = hPtr;
        ev->evType = JSI_EVENT_ALWAYS;
        Jsi_HashValueSet(hPtr, ev);
        interp->EventHdlId = id;
    }
}