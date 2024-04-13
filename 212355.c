static int parse(uint32_t *sizemax)
{
    long apos = 0;
    long aposmax = ftell(g_fin) + *sizemax;
    uint32_t size;

    if (g_atom->opcode != ATOM_NAME)
    {
        fprintf(stderr, "parse error: root is not a 'name' opcode\n");
        return ERR_FAIL;
    }
    //fprintf(stderr, "looking for '%s'\n", (char *)g_atom->data);

    // search for atom in the file
    while (1)
    {
        char name[4];
        uint32_t tmp;

        apos = ftell(g_fin);
        if (apos >= (aposmax - 8))
        {
            fprintf(stderr, "parse error: atom '%s' not found\n", (char *)g_atom->data);
            return ERR_FAIL;
        }
        if ((tmp = u32in()) < 8)
        {
            fprintf(stderr, "invalid atom size %x @%lx\n", tmp, ftell(g_fin));
            return ERR_FAIL;
        }

        size = tmp;
        if (datain(name, 4) != 4)
        {
            // EOF
            fprintf(stderr, "can't read atom name @%lx\n", ftell(g_fin));
            return ERR_FAIL;
        }

        //fprintf(stderr, "atom: '%c%c%c%c'(%x)", name[0],name[1],name[2],name[3], size);

        if (!memcmp(name, g_atom->data, 4))
        {
            //fprintf(stderr, "OK\n");
            break;
        }
        //fprintf(stderr, "\n");

        fseek(g_fin, apos + size, SEEK_SET);
    }
    *sizemax = size;
    g_atom++;
    if (g_atom->opcode == ATOM_DATA)
    {
        int err = ((int (*)(int)) g_atom->data)(size - 8);
        if (err < ERR_OK)
        {
            fseek(g_fin, apos + size, SEEK_SET);
            return err;
        }
        g_atom++;
    }
    if (g_atom->opcode == ATOM_DESCENT)
    {
        long apos = ftell(g_fin);;

        //fprintf(stderr, "descent\n");
        g_atom++;
        while (g_atom->opcode != ATOM_STOP)
        {
            uint32_t subsize = size - 8;
            int ret;
            if (g_atom->opcode == ATOM_ASCENT)
            {
                g_atom++;
                break;
            }
            fseek(g_fin, apos, SEEK_SET);
            if ((ret = parse(&subsize)) < 0)
                return ret;
        }
        //fprintf(stderr, "ascent\n");
    }

    fseek(g_fin, apos + size, SEEK_SET);

    return ERR_OK;
}