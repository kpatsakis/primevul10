static BIGNUM *getBN(STANZA *s, const char *attribute)
{
    const char *hex;
    BIGNUM *ret = NULL;

    if ((hex = findattr(s, attribute)) == NULL) {
        TEST_error("%s:%d: Can't find %s", s->test_file, s->start, attribute);
        return NULL;
    }

    if (parseBN(&ret, hex) != (int)strlen(hex)) {
        TEST_error("Could not decode '%s'", hex);
        return NULL;
    }
    return ret;
}