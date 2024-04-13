ofpact_is_allowed_in_actions_set(const struct ofpact *a)
{
    return action_set_classify(a) != ACTION_SLOT_INVALID;
}