static const char *findattr(STANZA *s, const char *key)
{
    int i = s->numpairs;
    PAIR *pp = s->pairs;

    for ( ; --i >= 0; pp++)
        if (strcasecmp(pp->key, key) == 0)
            return pp->value;
    return NULL;
}