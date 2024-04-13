DriverStatsObjCmd(ClientData UNUSED(clientData), Tcl_Interp *interp, int objc, Tcl_Obj *const* objv)
{
    int result = TCL_OK;

    if (Ns_ParseObjv(NULL, NULL, interp, 2, objc, objv) != NS_OK) {
        result = TCL_ERROR;

    } else {

        const Driver *drvPtr;
        Tcl_Obj      *resultObj = Tcl_NewListObj(0, NULL);

        /*
         * Iterate over all drivers and collect results.
         */
        for (drvPtr = firstDrvPtr; drvPtr != NULL;  drvPtr = drvPtr->nextPtr) {
            Tcl_Obj *listObj = Tcl_NewListObj(0, NULL);


            Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("thread", 6));
            Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj(drvPtr->threadName, -1));

            Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("module", 6));
            Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj(drvPtr->moduleName, -1));

            Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("received", 8));
            Tcl_ListObjAppendElement(interp, listObj, Tcl_NewWideIntObj(drvPtr->stats.received));

            Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("spooled", 7));
            Tcl_ListObjAppendElement(interp, listObj, Tcl_NewWideIntObj(drvPtr->stats.spooled));

            Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("partial", 7));
            Tcl_ListObjAppendElement(interp, listObj, Tcl_NewWideIntObj(drvPtr->stats.partial));

            Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("errors", 6));
            Tcl_ListObjAppendElement(interp, listObj, Tcl_NewWideIntObj(drvPtr->stats.errors));

            Tcl_ListObjAppendElement(interp, resultObj, listObj);
        }
        Tcl_SetObjResult(interp, resultObj);
    }

    return result;
}