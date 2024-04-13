ofpacts_execute_action_set(struct ofpbuf *action_list,
                           const struct ofpbuf *action_set)
{
    const struct ofpact *slots[N_ACTION_SLOTS] = {NULL, };

    struct ofpbuf set_or_move;
    ofpbuf_init(&set_or_move, 0);

    const struct ofpact *final_action = NULL;
    enum action_set_class final_class = 0;

    const struct ofpact *cursor;
    OFPACT_FOR_EACH (cursor, action_set->data, action_set->size) {
        int class = action_set_classify(cursor);
        if (class < N_ACTION_SLOTS) {
            slots[class] = cursor;
        } else if (class < ACTION_SLOT_SET_OR_MOVE) {
            if (class >= final_class) {
                final_action = cursor;
                final_class = class;
            }
        } else if (class == ACTION_SLOT_SET_OR_MOVE) {
            ofpact_copy(&set_or_move, cursor);
        } else {
            ovs_assert(class == ACTION_SLOT_INVALID);
        }
    }

    if (final_action) {
        for (int i = 0; i < N_ACTION_SLOTS; i++) {
            if (slots[i]) {
                ofpact_copy(action_list, slots[i]);
            }
        }
        ofpbuf_put(action_list, set_or_move.data, set_or_move.size);
        ofpact_copy(action_list, final_action);
    }
    ofpbuf_uninit(&set_or_move);
}