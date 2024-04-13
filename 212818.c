static void *_clua_allocator(void *ud, void *ptr, size_t osize, size_t nsize)
{
    CLua *cl = static_cast<CLua *>(ud);
    cl->memory_used += nsize - osize;

    if (nsize > osize && cl->memory_used >= CLUA_MAX_MEMORY_USE * 1024
        && cl->mixed_call_depth)
    {
        return nullptr;
    }

    if (!nsize)
    {
        free(ptr);
        return nullptr;
    }
    else
        return realloc(ptr, nsize);
}