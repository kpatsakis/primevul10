static void character_pushback(struct gps_packet_t *lexer)
/* push back the last character grabbed */
{
    /*@-modobserver@*//* looks like a splint bug */
    --lexer->inbufptr;
    /*@+modobserver@*/
    --lexer->char_counter;
    gpsd_report(LOG_RAW + 2, "%08ld: character pushed back\n",
		lexer->char_counter);
}