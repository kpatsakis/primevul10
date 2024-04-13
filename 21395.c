static void authentication_failed(VncState *vs)
{
    vnc_write_u32(vs, 1); /* Reject auth */
    if (vs->minor >= 8) {
        static const char err[] = "Authentication failed";
        vnc_write_u32(vs, sizeof(err));
        vnc_write(vs, err, sizeof(err));
    }
    vnc_flush(vs);
    vnc_client_error(vs);
}