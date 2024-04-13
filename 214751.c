connection_write_throttle (connection * const con, off_t max_bytes)
{
    /*assert(max_bytes > 0);*/
    max_bytes = connection_write_throttled(con, max_bytes);
    if (0 == max_bytes) con->traffic_limit_reached = 1;
    return max_bytes;
}