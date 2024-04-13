static void zrle_write_u16(VncState *vs, uint16_t value)
{
    vnc_write(vs, (uint8_t *)&value, 2);
}