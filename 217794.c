check_LEARN(const struct ofpact_learn *a,
            const struct ofpact_check_params *cp)
{
    return learn_check(a, cp->match);
}