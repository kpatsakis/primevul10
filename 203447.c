static void packet_accept(struct gps_packet_t *lexer, int packet_type)
/* packet grab succeeded, move to output buffer */
{
    size_t packetlen = lexer->inbufptr - lexer->inbuffer;
    if (packetlen < sizeof(lexer->outbuffer)) {
	memcpy(lexer->outbuffer, lexer->inbuffer, packetlen);
	lexer->outbuflen = packetlen;
	lexer->outbuffer[packetlen] = '\0';
	lexer->type = packet_type;
	if (lexer->debug >= LOG_RAW+1)
	    gpsd_report(LOG_RAW+1, "Packet type %d accepted %zu = %s\n",
		    packet_type, packetlen,
			gpsd_packetdump((char *)lexer->outbuffer, lexer->outbuflen));
    } else {
	gpsd_report(LOG_ERROR, "Rejected too long packet type %d len %zu\n",
		    packet_type, packetlen);
    }
}