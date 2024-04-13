zrenamefile(i_ctx_t *i_ctx_p)
{
    int code;
    os_ptr op = osp;
    gs_parsed_file_name_t pname1, pname2;

    code = parse_real_file_name(op, &pname2, imemory, "renamefile(to)");
    if (code < 0)
        return code;

    pname1.fname = 0;
    code = parse_real_file_name(op - 1, &pname1, imemory, "renamefile(from)");
    if (code >= 0) {
        gx_io_device *iodev_dflt = iodev_default(imemory);
        if (pname1.iodev != pname2.iodev ) {
            if (pname1.iodev == iodev_dflt)
                pname1.iodev = pname2.iodev;
            if (pname2.iodev == iodev_dflt)
                pname2.iodev = pname1.iodev;
        }
        if (pname1.iodev != pname2.iodev ||
            (pname1.iodev == iodev_dflt &&
                /*
                 * We require FileControl permissions on the source path
                 * unless it is a temporary file. Also, we require FileControl
                 * and FileWriting permissions to the destination file/path.
                 */
              ((check_file_permissions(i_ctx_p, pname1.fname, pname1.len,
                                        "PermitFileControl") < 0 &&
                  !file_is_tempfile(i_ctx_p, op[-1].value.bytes, r_size(op - 1))) ||
              (check_file_permissions(i_ctx_p, pname2.fname, pname2.len,
                                        "PermitFileControl") < 0 ||
              check_file_permissions(i_ctx_p, pname2.fname, pname2.len,
                                        "PermitFileWriting") < 0 )))) {
            code = gs_note_error(gs_error_invalidfileaccess);
        } else {
            code = (*pname1.iodev->procs.rename_file)(pname1.iodev,
                            pname1.fname, pname2.fname);
        }
    }
    gs_free_file_name(&pname2, "renamefile(to)");
    gs_free_file_name(&pname1, "renamefile(from)");
    if (code < 0)
        return code;
    pop(2);
    return 0;
}