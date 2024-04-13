static const char *h2_ss_str(h2_stream_state_t state)
{
    switch (state) {
        case H2_SS_IDLE:
            return "IDLE";
        case H2_SS_RSVD_L:
            return "RESERVED_LOCAL";
        case H2_SS_RSVD_R:
            return "RESERVED_REMOTE";
        case H2_SS_OPEN:
            return "OPEN";
        case H2_SS_CLOSED_L:
            return "HALF_CLOSED_LOCAL";
        case H2_SS_CLOSED_R:
            return "HALF_CLOSED_REMOTE";
        case H2_SS_CLOSED:
            return "CLOSED";
        case H2_SS_CLEANUP:
            return "CLEANUP";
        default:
            return "UNKNOWN";
    }
}