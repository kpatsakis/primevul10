static int mdhdin(int size)
{
    // version/flags
    u32in();
    // Creation time
    mp4config.ctime = u32in();
    // Modification time
    mp4config.mtime = u32in();
    // Time scale
    mp4config.samplerate = u32in();
    // Duration
    mp4config.samples = u32in();
    // Language
    u16in();
    // pre_defined
    u16in();

    return size;
};