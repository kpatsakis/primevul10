int CLua::loadstring(const char *s, const char *context)
{
    return loadbuffer(s, strlen(s), context);
}