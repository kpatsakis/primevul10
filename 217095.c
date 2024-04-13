zlibfile(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    int code;
    byte cname[DEFAULT_BUFFER_SIZE];
    uint clen;
    gs_parsed_file_name_t pname;
    stream *s;
    gx_io_device *iodev_dflt;

    check_ostack(2);
    code = parse_file_name(op, &pname, i_ctx_p->LockFilePermissions, imemory);
    if (code < 0)
        return code;
    iodev_dflt = iodev_default(imemory);
    if (pname.iodev == NULL)
        pname.iodev = iodev_dflt;
    if (pname.iodev != iodev_dflt) { /* Non-OS devices don't have search paths (yet). */
        code = zopen_file(i_ctx_p, &pname, "r", &s, imemory);
        if (code >= 0) {
            code = ssetfilename(s, op->value.const_bytes, r_size(op));
            if (code < 0) {
                sclose(s);
                return_error(gs_error_VMerror);
            }
        }
        if (code < 0) {
            push(1);
            make_false(op);
            return 0;
        }
        make_stream_file(op, s, "r");
    } else {
        ref fref;

        code = lib_file_open(i_ctx_p->lib_path, imemory, i_ctx_p, pname.fname, pname.len,
                             (char *)cname, sizeof(cname), &clen, &fref);
        if (code >= 0) {
            s = fptr(&fref);
            code = ssetfilename(s, cname, clen);
            if (code < 0) {
                sclose(s);
                return_error(gs_error_VMerror);
            }
        }
        if (code < 0) {
            if (code == gs_error_VMerror || code == gs_error_invalidfileaccess)
                return code;
            push(1);
            make_false(op);
            return 0;
        }
        ref_assign(op, &fref);
    }
    push(1);
    make_true(op);
    return 0;
}