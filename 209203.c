DriverNamesObjCmd(ClientData UNUSED(clientData), Tcl_Interp *interp, int objc, Tcl_Obj *const* objv)
{
    int result = TCL_OK;

    if (Ns_ParseObjv(NULL, NULL, interp, 2, objc, objv) != NS_OK) {
        result = TCL_ERROR;

    } else {
        const Driver *drvPtr;
        Tcl_Obj      *resultObj = Tcl_NewListObj(0, NULL);
        Tcl_HashTable driverNames;     /* names of the drivers without duplicates */

        Tcl_InitHashTable(&driverNames, TCL_STRING_KEYS);

        /*
         * Iterate over all drivers and collect results.
         */
        for (drvPtr = firstDrvPtr; drvPtr != NULL;  drvPtr = drvPtr->nextPtr) {
            int            isNew;

            (void)Tcl_CreateHashEntry(&driverNames, drvPtr->moduleName, &isNew);
            if (isNew == 1) {
                Tcl_ListObjAppendElement(interp, resultObj, Tcl_NewStringObj(drvPtr->moduleName, -1));
            }
        }
        Tcl_SetObjResult(interp, resultObj);
        Tcl_DeleteHashTable(&driverNames);
    }

    return result;
}