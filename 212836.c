void CLua::save_persist()
{
    string persist;
    // We load persist.lua immediately before calling c_save_persist so
    // that we know that it hasn't been overwritten by a player version.
    execfile("dlua/persist.lua", true, true);
    callfn("c_save_persist", ">s", &persist);
    if (Options.no_save)
        return;

    FILE *f;
    const string persistfile = _get_persist_file();

    // Don't create the file if there's no need to do so.
    if (persist.empty() && !file_exists(persistfile))
        return;

    f = fopen_u(persistfile.c_str(), "w");
    if (!f)
    {
        mprf(MSGCH_ERROR, "Couldn't open %s for writing!", persistfile.c_str());
        return;
    }

    fprintf(f, "-- %s %s persistent clua file\n"
               "-- WARNING: This file is entirely auto-generated.\n"
            "\n",
            OUTS(CRAWL), // ok, localizing the game name is not likely
            OUTS(Version::Long)); // nor the version string
    fprintf(f, "%s", persist.c_str());
    fclose(f);
}