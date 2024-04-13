parse_file_name(const ref * op, gs_parsed_file_name_t * pfn, bool safemode,
                gs_memory_t *memory)
{
    int code;

    check_read_type(*op, t_string);
    code = gs_parse_file_name(pfn, (const char *)op->value.const_bytes,
                              r_size(op), memory);
    if (code < 0)
        return code;
    /*
     * Check here for the %pipe device which is illegal when
     * LockFilePermissions is true. In the future we might want to allow
     * the %pipe device to be included on the PermitFile... paths, but
     * for now it is simply disallowed.
     */
    if (pfn->iodev && safemode && strcmp(pfn->iodev->dname, "%pipe%") == 0)
        return gs_error_invalidfileaccess;
    return code;
}