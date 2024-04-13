connection_write_throttled (const connection * const con, off_t max_bytes)
{
    const request_config * const restrict rconf = &con->request.conf;
    if (0 == rconf->global_bytes_per_second && 0 == rconf->bytes_per_second)
        return max_bytes;

    if (rconf->global_bytes_per_second) {
        off_t limit = (off_t)rconf->global_bytes_per_second
                    - *(rconf->global_bytes_per_second_cnt_ptr);
        if (max_bytes > limit)
            max_bytes = limit;
    }

    if (rconf->bytes_per_second) {
        off_t limit = (off_t)rconf->bytes_per_second
                    - con->bytes_written_cur_second;
        if (max_bytes > limit)
            max_bytes = limit;
    }

    return max_bytes > 0 ? max_bytes : 0; /*(0 == reached traffic limit)*/
}