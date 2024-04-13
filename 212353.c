static int tagu32(char *tagname, int n /*number of stored fields*/)
{
    //int numsize = n * 4;
    int size = 0;
    //int datasize = numsize + 16;

#if 0
    size += u32out(datasize + 8);
    size += dataout(tagname, 4);
    size += u32out(datasize);
    size += dataout("data", 4);
    size += u32out(0);
    size += u32out(0);
#endif

    return size;
}