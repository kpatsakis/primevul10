Section_t * CreateSection(int SectionType, unsigned char * Data, int Size)
{
    Section_t * NewSection;
    int a;
    int NewIndex;

    NewIndex = 0; // Figure out where to put the comment section.
    if (SectionType == M_EXIF){
        // Exif alwas goes first!
    }else{
        for (;NewIndex < 3;NewIndex++){ // Maximum fourth position (just for the heck of it)
            if (Sections[NewIndex].Type == M_JFIF) continue; // Put it after Jfif
            if (Sections[NewIndex].Type == M_EXIF) continue; // Put it after Exif
            break;
        }
    }

    if (SectionsRead < NewIndex){
        ErrFatal("Too few sections!");
    }

    CheckSectionsAllocated();
    for (a=SectionsRead;a>NewIndex;a--){
        Sections[a] = Sections[a-1];          
    }
    SectionsRead += 1;

    NewSection = Sections+NewIndex;

    NewSection->Type = SectionType;
    NewSection->Size = Size;
    NewSection->Data = Data;

    return NewSection;
}