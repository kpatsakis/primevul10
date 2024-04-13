static void zrle_write_u24b(VncState *vs, uint32_t value)
{
    vnc_write(vs, ((uint8_t *)&value) + 1, 3);
}