zfile(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    char file_access[4];
    gs_parsed_file_name_t pname;
    int code = parse_file_access_string(op, file_access);
    stream *s;

    if (code < 0)
        return code;
    code = parse_file_name(op-1, &pname, i_ctx_p->LockFilePermissions, imemory);
    if (code < 0)
        return code;
        /*
         * HACK: temporarily patch the current context pointer into the
         * state pointer for stdio-related devices.  See ziodev.c for
         * more information.
         */
    if (pname.iodev && pname.iodev->dtype == iodev_dtype_stdio) {
        bool statement = (strcmp(pname.iodev->dname, "%statementedit%") == 0);
        bool lineedit = (strcmp(pname.iodev->dname, "%lineedit%") == 0);
        if (pname.fname)
            return_error(gs_error_invalidfileaccess);
        if (statement || lineedit) {
            /* These need special code to support callouts */
            gx_io_device *indev = gs_findiodevice(imemory,
                                                  (const byte *)"%stdin", 6);
            stream *ins;
            if (strcmp(file_access, "r"))
                return_error(gs_error_invalidfileaccess);
            indev->state = i_ctx_p;
            code = (indev->procs.open_device)(indev, file_access, &ins, imemory);
            indev->state = 0;
            if (code < 0)
                return code;
            check_ostack(2);
            push(2);
            make_stream_file(op - 3, ins, file_access);
            make_bool(op-2, statement);
            make_int(op-1, 0);
            make_string(op, icurrent_space, 0, NULL);
            return zfilelineedit(i_ctx_p);
        }
        pname.iodev->state = i_ctx_p;
        code = (*pname.iodev->procs.open_device)(pname.iodev,
                                                 file_access, &s, imemory);
        pname.iodev->state = NULL;
    } else {
        if (pname.iodev == NULL)
            pname.iodev = iodev_default(imemory);
        code = zopen_file(i_ctx_p, &pname, file_access, &s, imemory);
    }
    if (code < 0)
        return code;
    code = ssetfilename(s, op[-1].value.const_bytes, r_size(op - 1));
    if (code < 0) {
        sclose(s);
        return_error(gs_error_VMerror);
    }
    make_stream_file(op - 1, s, file_access);
    pop(1);
    return code;
}