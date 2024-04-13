parse_real_file_name(const ref *op, gs_parsed_file_name_t *pfn,
                     gs_memory_t *mem, client_name_t cname)
{
    check_read_type(*op, t_string);
    return gs_parse_real_file_name(pfn, (const char *)op->value.const_bytes,
                                   r_size(op), mem, cname);
}