lib_file_open_search_with_no_combine(gs_file_path_ptr  lib_path, const gs_memory_t *mem, i_ctx_t *i_ctx_p,
                                     const char *fname, uint flen, char *buffer, int blen, uint *pclen, ref *pfile,
                                     gx_io_device *iodev, bool starting_arg_file, char *fmode)
{
    stream *s;
    uint blen1 = blen;
    struct stat fstat;

    if (gp_file_name_reduce(fname, flen, buffer, &blen1) != gp_combine_success)
      goto skip;

    if (starting_arg_file || check_file_permissions_aux(i_ctx_p, buffer, blen1) >= 0) {
        if (iodev_os_open_file(iodev, (const char *)buffer, blen1,
                       (const char *)fmode, &s, (gs_memory_t *)mem) == 0) {
            *pclen = blen1;
            make_stream_file(pfile, s, "r");
            return 0;
        }
    }
    else {
        /* If we are not allowed to open the file by check_file_permissions_aux()
         * and if the file exists, throw an error.......
         * Otherwise, keep searching.
         */
        if ((*iodev->procs.file_status)(iodev,  buffer, &fstat) >= 0) {
            return_error(gs_error_invalidfileaccess);
        }
    }

 skip:
    return 1;
}