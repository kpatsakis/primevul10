static void zrle_write_u8(VncState *vs, uint8_t value)
{
    vnc_write_u8(vs, value);
}