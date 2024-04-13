static void packet_discard(struct gps_packet_t *lexer)
/* shift the input buffer to discard all data up to current input pointer */
{
    size_t discard = lexer->inbufptr - lexer->inbuffer;
    size_t remaining = lexer->inbuflen - discard;
    lexer->inbufptr = memmove(lexer->inbuffer, lexer->inbufptr, remaining);
    lexer->inbuflen = remaining;
    if (lexer->debug >= LOG_RAW+1)
	gpsd_report(LOG_RAW + 1,
		    "Packet discard of %zu, chars remaining is %zu = %s\n",
		    discard, remaining,
		    gpsd_packetdump((char *)lexer->inbuffer, lexer->inbuflen));
}