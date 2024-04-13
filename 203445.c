void packet_init( /*@out@*/ struct gps_packet_t *lexer)
{
    lexer->char_counter = 0;
    lexer->retry_counter = 0;
#ifdef PASSTHROUGH_ENABLE
    lexer->json_depth = 0;
#endif /* PASSTHROUGH_ENABLE */
#ifdef TIMING_ENABLE
    lexer->start_time = 0.0;
#endif /* TIMING_ENABLE */
    packet_reset(lexer);
}