static void skip_header_chunk(EXRContext *s)
{
    GetByteContext *gb = &s->gb;

    while (bytestream2_get_bytes_left(gb) > 0) {
        if (!bytestream2_peek_byte(gb))
            break;

        // Process unknown variables
        for (int i = 0; i < 2; i++) // value_name and value_type
            while (bytestream2_get_byte(gb) != 0);

        // Skip variable length
        bytestream2_skip(gb, bytestream2_get_le32(gb));
    }
}