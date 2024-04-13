static void zrle_write_u32(VncState *vs, uint32_t value)
{
    vnc_write(vs, (uint8_t *)&value, 4);
}