void packet_pushback(struct gps_packet_t *lexer)
/* push back the last packet grabbed */
{
    if (lexer->outbuflen + lexer->inbuflen < MAX_PACKET_LENGTH) {
	memmove(lexer->inbuffer + lexer->outbuflen,
		lexer->inbuffer, lexer->inbuflen);
	memmove(lexer->inbuffer, lexer->outbuffer, lexer->outbuflen);
	lexer->inbuflen += lexer->outbuflen;
	lexer->inbufptr += lexer->outbuflen;
	lexer->outbuflen = 0;
    }
}