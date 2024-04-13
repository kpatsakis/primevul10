WriterCheckInputParams(Tcl_Interp *interp, const char *filenameString,
                       size_t size, off_t offset,
                       int *fdPtr, size_t *nrbytesPtr)
{
    int         result = TCL_OK, rc;
    struct stat st;

    Ns_Log(DriverDebug, "WriterCheckInputParams %s offset %" PROTd " size %" PRIdz,
           filenameString, offset, size);

    /*
     * Use stat() call to obtain information about the actual file to check
     * later the plausibility of the parameters.
     */
    rc = stat(filenameString, &st);
    if (unlikely(rc != 0)) {
        Ns_TclPrintfResult(interp, "file does not exist '%s'", filenameString);
        result = TCL_ERROR;

    } else {
        size_t nrbytes = 0u;
        int    fd;

        /*
         * Try to open the file and check offset and size parameters.
         */
        fd = ns_open(filenameString, O_RDONLY | O_CLOEXEC, 0);

        if (unlikely(fd == NS_INVALID_FD)) {
            Ns_TclPrintfResult(interp, "could not open file '%s'", filenameString);
            result = TCL_ERROR;

        } else if (unlikely(offset > st.st_size) || offset < 0) {
            Ns_TclPrintfResult(interp, "offset must be a positive value less or equal filesize");
            result = TCL_ERROR;

        } else if (size > 0) {
            if (unlikely((off_t)size + offset > st.st_size)) {
                Ns_TclPrintfResult(interp, "offset + size must be less or equal filesize");
                result = TCL_ERROR;
            } else {
                nrbytes = (size_t)size;
            }
        } else {
            nrbytes = (size_t)st.st_size - (size_t)offset;
        }

        /*
         * When an offset is provide, jump to this offset.
         */
        if (offset > 0 && result == TCL_OK) {
            if (ns_lseek(fd, (off_t)offset, SEEK_SET) == -1) {
                Ns_TclPrintfResult(interp, "cannot seek to position %ld", (long)offset);
                result = TCL_ERROR;
            }
        }

        if (result == TCL_OK) {
            *fdPtr = fd;
            *nrbytesPtr = nrbytes;

        } else if (fd != NS_INVALID_FD) {
            /*
             * On invalid parameters, close the fd.
             */
            ns_close(fd);
        }
    }

    return result;
}