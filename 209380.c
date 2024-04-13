static void IterObjInsertKey(Jsi_IterObj *io, const char *key)
{
    assert(!io->isArrayList);
    if (io->depth) {
        uint i;
        for (i=0; i<io->count; i++) {
            if (!Jsi_Strcmp(key, io->keys[i]))
                return;
        }
    }

    if (io->count >= io->size) {
        io->size += 10;
        io->keys = (const char**)Jsi_Realloc(io->keys, io->size * sizeof(io->keys[0]));
    }
    io->keys[io->count] = key;
    io->count++;
}