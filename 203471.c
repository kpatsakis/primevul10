static bool tnt_speed(struct gps_device_t *session,
		      speed_t speed, char parity UNUSED, int stopbits UNUSED)
{
    /*
     * Baud rate change followed by device reset.
     * See page 40 of Technical Guide 1555-B.  We need:
     * 2400 -> 1, 4800 -> 2, 9600 -> 3, 19200 -> 4, 38400 -> 5
     */
    unsigned int val = speed / 2400u;	/* 2400->1, 4800->2, 9600->4, 19200->8...  */
    unsigned int i = 0;

    /* fast way to compute log2(val) */
    while ((val >> i) > 1)
	++i;
    return tnt_send(session, "@B6=%d", i + 1)
	&& tnt_send(session, "@F28.6=1");
}