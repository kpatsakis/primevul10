DriverInfoObjCmd(ClientData UNUSED(clientData), Tcl_Interp *interp, int objc, Tcl_Obj *const* objv)
{
    int result = TCL_OK;

    if (Ns_ParseObjv(NULL, NULL, interp, 2, objc, objv) != NS_OK) {
        result = TCL_ERROR;

    } else {
        const Driver *drvPtr;
        Tcl_Obj      *resultObj = Tcl_NewListObj(0, NULL);
        Tcl_HashTable driverNames;     /* names of the driver modules without duplicates */

        Tcl_InitHashTable(&driverNames, TCL_STRING_KEYS);

        /*
         * Iterate over all modules, not necessarily all driver threads
         */
        for (drvPtr = firstDrvPtr; drvPtr != NULL;  drvPtr = drvPtr->nextPtr) {
            int isNew = 0;

            (void)Tcl_CreateHashEntry(&driverNames, drvPtr->moduleName, &isNew);
            if (isNew == 1) {
                Tcl_Obj *listObj = Tcl_NewListObj(0, NULL);

                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("module", 6));
                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj(drvPtr->moduleName, -1));

                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("type", 4));
                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj(drvPtr->type, -1));

                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("server", 6));
                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj(drvPtr->server != NULL ?
                                                                           drvPtr->server : NS_EMPTY_STRING, -1));

                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("location", 8));
                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj(drvPtr->location, -1));

                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("address", 7));
                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj(drvPtr->address, -1));

                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("protocol", 8));
                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj(drvPtr->protocol, -1));

                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("sendwait", 8));
                Tcl_ListObjAppendElement(interp, listObj, Ns_TclNewTimeObj(&drvPtr->sendwait));

                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("recvwait", 8));
                Tcl_ListObjAppendElement(interp, listObj, Ns_TclNewTimeObj(&drvPtr->sendwait));

                Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("extraheaders", 12));
                if (drvPtr->extraHeaders != NULL) {
                    Tcl_DString ds;

                    Tcl_DStringInit(&ds);
                    Ns_DStringAppendSet(&ds, drvPtr->extraHeaders);
                    Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj(ds.string, ds.length));
                    Tcl_DStringFree(&ds);
                } else {
                    Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("", 0));
                }

                Tcl_ListObjAppendElement(interp, resultObj, listObj);
            }
        }
        Tcl_SetObjResult(interp, resultObj);
        Tcl_DeleteHashTable(&driverNames);
    }

    return result;
}