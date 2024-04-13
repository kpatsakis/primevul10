zfilenameforall(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    file_enum *pfen;
    gx_io_device *iodev = NULL;
    gs_parsed_file_name_t pname;
    int code = 0;

    check_write_type(*op, t_string);
    check_proc(op[-1]);
    check_read_type(op[-2], t_string);
    /* Push a mark, the iodev, devicenamelen, the scratch string, the enumerator, */
    /* and the procedure, and invoke the continuation. */
    check_estack(7);
    /* Get the iodevice */
    code = parse_file_name(op-2, &pname, i_ctx_p->LockFilePermissions, imemory);
    if (code < 0)
        return code;
    iodev = (pname.iodev == NULL) ? iodev_default(imemory) : pname.iodev;

    /* Check for several conditions that just cause us to return success */
    if (pname.len == 0 || iodev->procs.enumerate_files == iodev_no_enumerate_files) {
        pop(3);
        return 0;       /* no pattern, or device not found -- just return */
    }
    pfen = iodev->procs.enumerate_files(iodev, (const char *)pname.fname,
                pname.len, imemory);
    if (pfen == 0)
        return_error(gs_error_VMerror);
    push_mark_estack(es_for, file_cleanup);
    ++esp;
    make_istruct(esp, 0, iodev);
    ++esp;
    make_int(esp, r_size(op-2) - pname.len);
    *++esp = *op;
    ++esp;
    make_istruct(esp, 0, pfen);
    *++esp = op[-1];
    pop(3);
    code = file_continue(i_ctx_p);
    return (code == o_pop_estack ? o_push_estack : code);
}