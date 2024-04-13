static void character_discard(struct gps_packet_t *lexer)
/* shift the input buffer to discard one character and reread data */
{
    memmove(lexer->inbuffer, lexer->inbuffer + 1, (size_t)-- lexer->inbuflen);
    lexer->inbufptr = lexer->inbuffer;
    if (lexer->debug >= LOG_RAW+1)
	gpsd_report(LOG_RAW + 1, "Character discarded, buffer %zu chars = %s\n",
		    lexer->inbuflen,
		    gpsd_packetdump((char *)lexer->inbuffer, lexer->inbuflen));
}