static int on_map(h2_stream_state_t state, int map[H2_SS_MAX])
{
    int op = map[state];
    switch (op) {
        case S_XXX:
        case S_ERR:
            return op;
        case S_NOP:
            return state;
        default:
            return op-1;
    }
}