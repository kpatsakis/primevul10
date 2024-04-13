ssize_t packet_get(int fd, struct gps_packet_t *lexer)
/* grab a packet; return -1=>I/O error, 0=>EOF, BAD_PACKET or a length */
{
    ssize_t recvd;

    /*@ -modobserver @*/
    errno = 0;
    recvd = read(fd, lexer->inbuffer + lexer->inbuflen,
		 sizeof(lexer->inbuffer) - (lexer->inbuflen));
    /*@ +modobserver @*/
    if (recvd == -1) {
	if ((errno == EAGAIN) || (errno == EINTR)) {
	    gpsd_report(LOG_RAW + 2, "no bytes ready\n");
	    recvd = 0;
	    /* fall through, input buffer may be nonempty */
	} else {
	    gpsd_report(LOG_RAW + 2, "errno: %s\n", strerror(errno));
	    return -1;
	}
    } else {
	if (lexer->debug >= LOG_RAW+1)
	    gpsd_report(LOG_RAW + 1,
			"Read %zd chars to buffer offset %zd (total %zd): %s\n",
			recvd, lexer->inbuflen, lexer->inbuflen + recvd,
			gpsd_packetdump((char *)lexer->inbufptr, (size_t) recvd));
	lexer->inbuflen += recvd;
    }
    gpsd_report(LOG_SPIN, "packet_get() fd %d -> %zd (%d)\n",
		fd, recvd, errno);

    /*
     * Bail out, indicating no more input, only if we just received
     * nothing from the device and there is nothing waiting in the
     * packet input buffer.
     */
    if (recvd <= 0 && packet_buffered_input(lexer) <= 0)
	return recvd;

    /* Otherwise, consume from the packet input buffer */
    /* coverity[tainted_data] */
    packet_parse(lexer);

    /* if input buffer is full, discard */
    if (sizeof(lexer->inbuffer) == (lexer->inbuflen)) {
	/* coverity[tainted_data] */
	packet_discard(lexer);
	lexer->state = GROUND_STATE;
    }

    /*
     * If we gathered a packet, return its length; it will have been
     * consumed out of the input buffer and moved to the output
     * buffer.  We don't care whether the read() returned 0 or -1 and
     * gathered packet data was all buffered or whether ot was partly
     * just physically read.
     *
     * Note: this choice greatly simplifies life for callers of
     * packet_get(), but means that they cannot tell when a nonzero
     * return means there was a successful physical read.  They will
     * thus credit a data source that drops out with being alive
     * slightly longer than it actually was.  This is unlikely to
     * matter as long as any policy timeouts are large compared to
     * the time required to consume the greatest possible amount
     * of buffered input, but if you hack this code you need to
     * be aware of the issue. It might also slightly affect
     * performance profiling.
     */
    if (lexer->outbuflen > 0)
	return (ssize_t) lexer->outbuflen;
    else
	/*
	 * Otherwise recvd is the size of whatever packet fragment we got.
	 * It can still be 0 or -1 at this point even if buffer data
	 * was consumed.
	 */
	return recvd;
}