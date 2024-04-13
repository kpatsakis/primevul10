static int ilstin(int size)
{
    enum {NUMSET = 1, GENRE, EXTAG};
    int read = 0;

    static struct {
        char *name;
        char *id;
        int flag;
    } tags[] = {
        {"Album       ", "\xa9" "alb"},
        {"Album Artist", "aART"},
        {"Artist      ", "\xa9" "ART"},
        {"Comment     ", "\xa9" "cmt"},
        {"Cover image ", "covr"},
        {"Compilation ", "cpil"},
        {"Copyright   ", "cprt"},
        {"Date        ", "\xa9" "day"},
        {"Disc#       ", "disk", NUMSET},
        {"Genre       ", "gnre", GENRE},
        {"Grouping    ", "\xa9" "grp"},
        {"Lyrics      ", "\xa9" "lyr"},
        {"Title       ", "\xa9" "nam"},
        {"Rating      ", "rtng"},
        {"BPM         ", "tmpo"},
        {"Encoder     ", "\xa9" "too"},
        {"Track       ", "trkn", NUMSET},
        {"Composer    ", "\xa9" "wrt"},
        {0, "----", EXTAG},
        {0},
    };

    static const char *genres[] = {
        "Blues", "Classic Rock", "Country", "Dance",
        "Disco", "Funk", "Grunge", "Hip-Hop",
        "Jazz", "Metal", "New Age", "Oldies",
        "Other", "Pop", "R&B", "Rap",
        "Reggae", "Rock", "Techno", "Industrial",
        "Alternative", "Ska", "Death Metal", "Pranks",
        "Soundtrack", "Euro-Techno", "Ambient", "Trip-Hop",
        "Vocal", "Jazz+Funk", "Fusion", "Trance",
        "Classical", "Instrumental", "Acid", "House",
        "Game", "Sound Clip", "Gospel", "Noise",
        "Alternative Rock", "Bass", "Soul", "Punk",
        "Space", "Meditative", "Instrumental Pop", "Instrumental Rock",
        "Ethnic", "Gothic", "Darkwave", "Techno-Industrial",
        "Electronic", "Pop-Folk", "Eurodance", "Dream",
        "Southern Rock", "Comedy", "Cult", "Gangsta",
        "Top 40", "Christian Rap", "Pop/Funk", "Jungle",
        "Native US", "Cabaret", "New Wave", "Psychadelic",
        "Rave", "Showtunes", "Trailer", "Lo-Fi",
        "Tribal", "Acid Punk", "Acid Jazz", "Polka",
        "Retro", "Musical", "Rock & Roll", "Hard Rock",
        "Folk", "Folk-Rock", "National Folk", "Swing",
        "Fast Fusion", "Bebob", "Latin", "Revival",
        "Celtic", "Bluegrass", "Avantgarde", "Gothic Rock",
        "Progressive Rock", "Psychedelic Rock", "Symphonic Rock", "Slow Rock",
        "Big Band", "Chorus", "Easy Listening", "Acoustic",
        "Humour", "Speech", "Chanson", "Opera",
        "Chamber Music", "Sonata", "Symphony", "Booty Bass",
        "Primus", "Porn Groove", "Satire", "Slow Jam",
        "Club", "Tango", "Samba", "Folklore",
        "Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle",
        "Duet", "Punk Rock", "Drum Solo", "Acapella",
        "Euro-House", "Dance Hall", "Goa", "Drum & Bass",
        "Club - House", "Hardcore", "Terror", "Indie",
        "BritPop", "Negerpunk", "Polsk Punk", "Beat",
        "Christian Gangsta Rap", "Heavy Metal", "Black Metal", "Crossover",
        "Contemporary Christian", "Christian Rock", "Merengue", "Salsa",
        "Thrash Metal", "Anime", "JPop", "Synthpop",
        "Unknown",
    };

    fprintf(stderr, "----------tag list-------------\n");
    while(read < size)
    {
        int asize, dsize;
        uint8_t id[5];
        int cnt;
        uint32_t type;

        id[4] = 0;

        asize = u32in();
        read += asize;
        asize -= 4;
        if (datain(id, 4) < 4)
            return ERR_FAIL;
        asize -= 4;

        for (cnt = 0; tags[cnt].id; cnt++)
        {
            if (!memcmp(id, tags[cnt].id, 4))
                break;
        }

        if (tags[cnt].name)
            fprintf(stderr, "%s :   ", tags[cnt].name);
        else
        {
            if (tags[cnt].flag != EXTAG)
                fprintf(stderr, "'%s'       :   ", id);
        }

        dsize = u32in();
        asize -= 4;
        if (datain(id, 4) < 4)
            return ERR_FAIL;
        asize -= 4;

        if (tags[cnt].flag != EXTAG)
        {
            if (memcmp(id, "data", 4))
                return ERR_FAIL;
        }
        else
        {
            int spc;

            if (memcmp(id, "mean", 4))
                goto skip;
            dsize -= 8;
            while (dsize > 0)
            {
                u8in();
                asize--;
                dsize--;
            }
            if (asize >= 8)
            {
                dsize = u32in() - 8;
                asize -= 4;
                if (datain(id, 4) < 4)
                    return ERR_FAIL;
                asize -= 4;
                if (memcmp(id, "name", 4))
                    goto skip;
                u32in();
                asize -= 4;
                dsize -= 4;
            }
            spc = 13 - dsize;
            if (spc < 0) spc = 0;
            while (dsize > 0)
            {
                fprintf(stderr, "%c",u8in());
                asize--;
                dsize--;
            }
            while (spc--)
                fprintf(stderr, " ");
            fprintf(stderr, ":   ");
            if (asize >= 8)
            {
                dsize = u32in() - 8;
                asize -= 4;
                if (datain(id, 4) < 4)
                    return ERR_FAIL;
                asize -= 4;
                if (memcmp(id, "data", 4))
                    goto skip;
                u32in();
                asize -= 4;
                dsize -= 4;
            }
            while (dsize > 0)
            {
                fprintf(stderr, "%c",u8in());
                asize--;
                dsize--;
            }
            fprintf(stderr, "\n");

            goto skip;
        }
        type = u32in();
        asize -= 4;
        u32in();
        asize -= 4;

        switch(type)
        {
        case 1:
            while (asize > 0)
            {
                fprintf(stderr, "%c",u8in());
                asize--;
            }
            break;
        case 0:
            switch(tags[cnt].flag)
            {
            case NUMSET:
                u16in();
                asize -= 2;

                fprintf(stderr, "%d", u16in());
                asize -= 2;
                fprintf(stderr, "/%d", u16in());
                asize -= 2;
                break;
            case GENRE:
                {
                    uint8_t gnum = u16in();
                    asize -= 2;
                    if (!gnum)
                       goto skip;
                    gnum--;
                    if (gnum >= 147)
                        gnum = 147;
                    fprintf(stderr, "%s", genres[gnum]);
                }
                break;
            default:
                while(asize > 0)
                {
                    fprintf(stderr, "%d/", u16in());
                    asize-=2;
                }
            }
            break;
        case 0x15:
            //fprintf(stderr, "(8bit data)");
            while(asize > 0)
            {
                fprintf(stderr, "%d", u8in());
                asize--;
                if (asize)
                    fprintf(stderr, "/");
            }
            break;
        case 0xd:
            fprintf(stderr, "(image data)");
            break;
        default:
            fprintf(stderr, "(unknown data type)");
            break;
        }
        fprintf(stderr, "\n");

    skip:
        // skip to the end of atom
        while (asize > 0)
        {
            u8in();
            asize--;
        }
    }
    fprintf(stderr, "-------------------------------\n");

    return size;
};