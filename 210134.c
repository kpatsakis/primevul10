void* Jsi_Malloc(unsigned int size) {
    void *v = malloc(size);
    Assert(v);
    return v;
}