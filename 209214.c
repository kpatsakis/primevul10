WriterSubmitFilesObjCmd(ClientData UNUSED(clientData), Tcl_Interp *interp, int objc, Tcl_Obj *const* objv)
{
    int         result = TCL_OK;
    Ns_Conn    *conn;
    int         headers = 0, nrFiles;
    Tcl_Obj    *filesObj = NULL, **fileObjv;
    Ns_ObjvSpec lopts[] = {
        {"-headers",  Ns_ObjvBool,    &headers, INT2PTR(NS_TRUE)},
        {NULL, NULL, NULL, NULL}
    };
    Ns_ObjvSpec args[] = {
        {"files",     Ns_ObjvObj, &filesObj, NULL},
        {NULL, NULL, NULL, NULL}
    };

    if (unlikely(Ns_ParseObjv(lopts, args, interp, 2, objc, objv) != NS_OK)
        || NsConnRequire(interp, NS_CONN_REQUIRE_ALL, &conn) != NS_OK) {
        result = TCL_ERROR;

    } else if (unlikely( Ns_ConnSockPtr(conn) == NULL )) {
        Ns_Log(Warning,
               "NsWriterQueue: called without valid sockPtr, "
               "maybe connection already closed");
        Ns_TclPrintfResult(interp, "0");
        result = TCL_OK;

    } else if (Tcl_ListObjGetElements(interp, filesObj, &nrFiles, &fileObjv) != TCL_OK) {
        Ns_TclPrintfResult(interp, "not a valid list of files: '%s'", Tcl_GetString(filesObj));
        result = TCL_ERROR;

    } else if (nrFiles == 0) {
        Ns_TclPrintfResult(interp, "The provided list has to contain at least one file spec");
        result = TCL_ERROR;

    } else {
        size_t      totalbytes = 0u, i;
        Tcl_Obj    *keys[3], *filenameObj = NULL;
        Ns_FileVec *filebufs;
        const char *firstFilenameString = NULL;
        Ns_ObjvValueRange offsetRange = {0, LLONG_MAX};
        Ns_ObjvValueRange sizeRange = {1, LLONG_MAX};

        filebufs = (Ns_FileVec *)ns_calloc((size_t)nrFiles, sizeof(Ns_FileVec));
        keys[0] = Tcl_NewStringObj("filename", 8);
        keys[1] = Tcl_NewStringObj("-offset", 7);
        keys[2] = Tcl_NewStringObj("-size", 5);

        Tcl_IncrRefCount(keys[0]);
        Tcl_IncrRefCount(keys[1]);
        Tcl_IncrRefCount(keys[2]);

        for (i = 0u; i < (size_t)nrFiles; i++) {
            filebufs[i].fd = NS_INVALID_FD;
        }

        /*
         * Iterate over the list of dicts.
         */
        for (i = 0u; i < (size_t)nrFiles; i++) {
            Tcl_WideInt offset = 0, size = 0;
            int         rc, fd = NS_INVALID_FD;
            const char *filenameString;
            size_t      nrbytes;

            /*
             * Get required "filename" element.
             */
            filenameObj = NULL;
            rc = Tcl_DictObjGet(interp, fileObjv[i], keys[0], &filenameObj);
            if (rc != TCL_OK || filenameObj == NULL) {
                Ns_TclPrintfResult(interp, "missing filename in dict '%s'",
                                   Tcl_GetString(fileObjv[i]));
                result = TCL_ERROR;
                break;
            }

            filenameString = Tcl_GetString(filenameObj);
            if (firstFilenameString == NULL) {
                firstFilenameString = filenameString;
            }

            /*
             * Get optional "-offset" and "-size" elements.
             */
            if (WriterGetMemunitFromDict(interp, fileObjv[i], keys[1], &offsetRange, &offset) != TCL_OK) {
                result = TCL_ERROR;
                break;
            }
            if (WriterGetMemunitFromDict(interp, fileObjv[i], keys[2], &sizeRange, &size) != TCL_OK) {
                result = TCL_ERROR;
                break;
            }

            /*
             * Check validity of the provided values
             */
            result = WriterCheckInputParams(interp, Tcl_GetString(filenameObj),
                                            (size_t)size, (off_t)offset,
                                            &fd, &nrbytes);
            if (result != TCL_OK) {
                break;
            }

            filebufs[i].fd = fd;
            filebufs[i].offset = offset;
            filebufs[i].length = nrbytes;

            totalbytes = totalbytes + (size_t)nrbytes;
        }
        Tcl_DecrRefCount(keys[0]);
        Tcl_DecrRefCount(keys[1]);
        Tcl_DecrRefCount(keys[2]);

        /*
         * If everything is ok, submit the request to the writer queue.
         */
        if (result == TCL_OK) {
            Ns_ReturnCode status;

            if (headers != 0 && firstFilenameString != NULL) {
                Ns_ConnSetTypeHeader(conn, Ns_GetMimeType(firstFilenameString));
            }
            status = NsWriterQueue(conn, totalbytes, NULL, NULL, NS_INVALID_FD, NULL, 0,
                                   filebufs, nrFiles, NS_TRUE);
            /*
             * Provide a soft error like for "ns_writer submitfile".
             */
            Tcl_SetObjResult(interp, Tcl_NewBooleanObj(status == NS_OK ? 1 : 0));
        }

        /*
         * The NsWriterQueue() API makes the usual duplicates of the file
         * descriptors and the Ns_FileVec structure, so we have to cleanup
         * here.
         */
        for (i = 0u; i < (size_t)nrFiles; i++) {
            if (filebufs[i].fd != NS_INVALID_FD) {
                (void) ns_close(filebufs[i].fd);
            }
        }
        ns_free(filebufs);

    }

    return result;
}