ofpacts_output_to_port(const struct ofpact *ofpacts, size_t ofpacts_len,
                       ofp_port_t port)
{
    const struct ofpact *a;

    OFPACT_FOR_EACH_FLATTENED (a, ofpacts, ofpacts_len) {
        if (ofpact_outputs_to_port(a, port)) {
            return true;
        }
    }

    return false;
}