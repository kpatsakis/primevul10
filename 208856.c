inline bool saveFile(const char* filename, const void* fileBytes, Uptr numFileBytes)
{
	Platform::File* file = Platform::openFile(
		filename, Platform::FileAccessMode::writeOnly, Platform::FileCreateMode::createAlways);
	if(!file)
	{
		Log::printf(Log::error, "Couldn't write %s: couldn't open file.\n", filename);
		return false;
	}

	errorUnless(Platform::writeFile(file, fileBytes, numFileBytes));
	errorUnless(Platform::closeFile(file));

	return true;
}