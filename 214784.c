bool pb_decode_fixed32(pb_istream_t *stream, void *dest)
{
    pb_byte_t bytes[4];

    if (!pb_read(stream, bytes, 4))
        return false;
    
    *(uint32_t*)dest = ((uint32_t)bytes[0] << 0) |
                       ((uint32_t)bytes[1] << 8) |
                       ((uint32_t)bytes[2] << 16) |
                       ((uint32_t)bytes[3] << 24);
    return true;
}