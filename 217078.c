zopen_file(i_ctx_t *i_ctx_p, const gs_parsed_file_name_t *pfn,
           const char *file_access, stream **ps, gs_memory_t *mem)
{
    gx_io_device *const iodev = pfn->iodev;
    int code = 0;

    if (pfn->fname == NULL) {     /* just a device */
        iodev->state = i_ctx_p;
        code = iodev->procs.open_device(iodev, file_access, ps, mem);
        iodev->state = NULL;
        return code;
    }
    else {                      /* file */
        iodev_proc_open_file((*open_file)) = iodev->procs.open_file;

        if (open_file == 0)
            open_file = iodev_os_open_file;
        /* Check OS files to make sure we allow the type of access */
        if (open_file == iodev_os_open_file) {
            code = check_file_permissions(i_ctx_p, pfn->fname, pfn->len,
                file_access[0] == 'r' ? "PermitFileReading" : "PermitFileWriting");

            if (code < 0 && !file_is_tempfile(i_ctx_p,
                                          (const uchar *)pfn->fname, pfn->len))
                return code;
        }
        return open_file(iodev, pfn->fname, pfn->len, file_access, ps, mem);
    }
}