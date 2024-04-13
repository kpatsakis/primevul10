void DiscardAllButExif(void)
{
    Section_t ExifKeeper;
    Section_t CommentKeeper;
    Section_t IptcKeeper;
    Section_t XmpKeeper;
    int a;

    memset(&ExifKeeper, 0, sizeof(ExifKeeper));
    memset(&CommentKeeper, 0, sizeof(CommentKeeper));
    memset(&IptcKeeper, 0, sizeof(IptcKeeper));
    memset(&XmpKeeper, 0, sizeof(IptcKeeper));

    for (a=0;a<SectionsRead;a++){
        if (Sections[a].Type == M_EXIF && ExifKeeper.Type == 0){
           ExifKeeper = Sections[a];
        }else if (Sections[a].Type == M_XMP && XmpKeeper.Type == 0){
           XmpKeeper = Sections[a];
        }else if (Sections[a].Type == M_COM && CommentKeeper.Type == 0){
            CommentKeeper = Sections[a];
        }else if (Sections[a].Type == M_IPTC && IptcKeeper.Type == 0){
            IptcKeeper = Sections[a];
        }else{
            free(Sections[a].Data);
        }
    }
    SectionsRead = 0;
    if (ExifKeeper.Type){
        CheckSectionsAllocated();
        Sections[SectionsRead++] = ExifKeeper;
    }
    if (CommentKeeper.Type){
        CheckSectionsAllocated();
        Sections[SectionsRead++] = CommentKeeper;
    }
    if (IptcKeeper.Type){
        CheckSectionsAllocated();
        Sections[SectionsRead++] = IptcKeeper;
    }

    if (XmpKeeper.Type){
        CheckSectionsAllocated();
        Sections[SectionsRead++] = XmpKeeper;
    }
}    