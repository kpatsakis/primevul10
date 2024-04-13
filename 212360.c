static int moovin(int sizemax)
{
    long apos = ftell(g_fin);
    uint32_t atomsize;
    creator_t *old_atom = g_atom;
    int err, ret = sizemax;

    static creator_t mvhd[] = {
        {ATOM_NAME, "mvhd"},
        {0}
    };
    static creator_t trak[] = {
        {ATOM_NAME, "trak"},
        {ATOM_DESCENT},
        {ATOM_NAME, "tkhd"},
        {ATOM_NAME, "mdia"},
        {ATOM_DESCENT},
        {ATOM_NAME, "mdhd"},
        {ATOM_DATA, mdhdin},
        {ATOM_NAME, "hdlr"},
        {ATOM_DATA, hdlr1in},
        {ATOM_NAME, "minf"},
        {ATOM_DESCENT},
        {ATOM_NAME, "smhd"},
        {ATOM_NAME, "dinf"},
        {ATOM_NAME, "stbl"},
        {ATOM_DESCENT},
        {ATOM_NAME, "stsd"},
        {ATOM_DATA, stsdin},
        {ATOM_DESCENT},
        {ATOM_NAME, "mp4a"},
        {ATOM_DATA, mp4ain},
        {ATOM_DESCENT},
        {ATOM_NAME, "esds"},
        {ATOM_DATA, esdsin},
        {ATOM_ASCENT},
        {ATOM_ASCENT},
        {ATOM_NAME, "stts"},
        {ATOM_DATA, sttsin},
        {ATOM_NAME, "stsc"},
        {ATOM_NAME, "stsz"},
        {ATOM_DATA, stszin},
        {ATOM_NAME, "stco"},
        {ATOM_DATA, stcoin},
        {0}
    };

    g_atom = mvhd;
    atomsize = sizemax + apos - ftell(g_fin);
    if (parse(&atomsize) < 0) {
        g_atom = old_atom;
        return ERR_FAIL;
    }

    fseek(g_fin, apos, SEEK_SET);

    while (1)
    {
        //fprintf(stderr, "TRAK\n");
        g_atom = trak;
        atomsize = sizemax + apos - ftell(g_fin);
        if (atomsize < 8)
            break;
        //fprintf(stderr, "PARSE(%x)\n", atomsize);
        err = parse(&atomsize);
        //fprintf(stderr, "SIZE: %x/%x\n", atomsize, sizemax);
        if (err >= 0)
            break;
        if (err != ERR_UNSUPPORTED) {
            ret = err;
            break;
        }
        //fprintf(stderr, "UNSUPP\n");
    }

    g_atom = old_atom;
    return ret;
}