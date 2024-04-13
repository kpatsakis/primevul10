bool pb_decode_fixed64(pb_istream_t *stream, void *dest)
{
    pb_byte_t bytes[8];

    if (!pb_read(stream, bytes, 8))
        return false;
    
    *(uint64_t*)dest = ((uint64_t)bytes[0] << 0) |
                       ((uint64_t)bytes[1] << 8) |
                       ((uint64_t)bytes[2] << 16) |
                       ((uint64_t)bytes[3] << 24) |
                       ((uint64_t)bytes[4] << 32) |
                       ((uint64_t)bytes[5] << 40) |
                       ((uint64_t)bytes[6] << 48) |
                       ((uint64_t)bytes[7] << 56);
    
    return true;
}