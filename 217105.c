lib_file_open_search_with_combine(gs_file_path_ptr  lib_path, const gs_memory_t *mem, i_ctx_t *i_ctx_p,
                                  const char *fname, uint flen, char *buffer, int blen, uint *pclen, ref *pfile,
                                  gx_io_device *iodev, bool starting_arg_file, char *fmode)
{
    stream *s;
    const gs_file_path *pfpath = lib_path;
    uint pi;

    for (pi = 0; pi < r_size(&pfpath->list); ++pi) {
        const ref *prdir = pfpath->list.value.refs + pi;
        const char *pstr = (const char *)prdir->value.const_bytes;
        uint plen = r_size(prdir), blen1 = blen;
        gs_parsed_file_name_t pname;
        gp_file_name_combine_result r;

        /* We need to concatenate and parse the file name here
         * if this path has a %device% prefix.              */
        if (pstr[0] == '%') {
            int code;

            /* We concatenate directly since gp_file_name_combine_*
             * rules are not correct for other devices such as %rom% */
            code = gs_parse_file_name(&pname, pstr, plen, mem);
            if (code < 0)
                continue;
            if (blen < max(pname.len, plen) + flen)
            	return_error(gs_error_limitcheck);
            memcpy(buffer, pname.fname, pname.len);
            memcpy(buffer+pname.len, fname, flen);
            code = pname.iodev->procs.open_file(pname.iodev, buffer, pname.len + flen, fmode,
                                          &s, (gs_memory_t *)mem);
            if (code < 0)
                continue;
            make_stream_file(pfile, s, "r");
            /* fill in the buffer with the device concatenated */
            memcpy(buffer, pstr, plen);
            memcpy(buffer+plen, fname, flen);
            *pclen = plen + flen;
            return 0;
        } else {
            r = gp_file_name_combine(pstr, plen,
                    fname, flen, false, buffer, &blen1);
            if (r != gp_combine_success)
                continue;
            if (iodev_os_open_file(iodev, (const char *)buffer, blen1, (const char *)fmode,
                                    &s, (gs_memory_t *)mem) == 0) {
                if (starting_arg_file ||
                    check_file_permissions_aux(i_ctx_p, buffer, blen1) >= 0) {
                    *pclen = blen1;
                    make_stream_file(pfile, s, "r");
                    return 0;
                }
                sclose(s);
                return_error(gs_error_invalidfileaccess);
            }
        }
    }
    return 1;
}