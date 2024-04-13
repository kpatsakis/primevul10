static void mp4info(void)
{
    fprintf(stderr, "Modification Time:\t\t%s\n", mp4time(mp4config.mtime));
    fprintf(stderr, "Samplerate:\t\t%d\n", mp4config.samplerate);
    fprintf(stderr, "Total samples:\t\t%d\n", mp4config.samples);
    fprintf(stderr, "Total channels:\t\t%d\n", mp4config.channels);
    fprintf(stderr, "Bits per sample:\t%d\n", mp4config.bits);
    fprintf(stderr, "Buffer size:\t\t%d\n", mp4config.buffersize);
    fprintf(stderr, "Max bitrate:\t\t%d\n", mp4config.bitratemax);
    fprintf(stderr, "Average bitrate:\t%d\n", mp4config.bitrateavg);
    fprintf(stderr, "Samples per frame:\t%d\n", mp4config.framesamples);
    fprintf(stderr, "Frames:\t\t\t%d\n", mp4config.frame.ents);
    fprintf(stderr, "ASC size:\t\t%d\n", mp4config.asc.size);
    fprintf(stderr, "Duration:\t\t%.1f sec\n", (float)mp4config.samples/mp4config.samplerate);
    fprintf(stderr, "Data offset/size:\t%x/%x\n", mp4config.mdatofs, mp4config.mdatsize);
}