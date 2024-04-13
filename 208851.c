inline bool loadFile(const char* filename, std::vector<U8>& outFileContents)
{
	Platform::File* file = Platform::openFile(
		filename, Platform::FileAccessMode::readOnly, Platform::FileCreateMode::openExisting);
	if(!file)
	{
		Log::printf(Log::error, "Couldn't read %s: couldn't open file.\n", filename);
		return false;
	}

	U64 numFileBytes = 0;
	errorUnless(Platform::seekFile(file, 0, Platform::FileSeekOrigin::end, &numFileBytes));
	if(numFileBytes > UINTPTR_MAX)
	{
		Log::printf(Log::error, "Couldn't read %s: file is too large.\n", filename);
		errorUnless(Platform::closeFile(file));
		return false;
	}

	std::vector<U8> fileContents;
	outFileContents.resize(numFileBytes);
	errorUnless(Platform::seekFile(file, 0, Platform::FileSeekOrigin::begin));
	errorUnless(Platform::readFile(file, const_cast<U8*>(outFileContents.data()), numFileBytes));
	errorUnless(Platform::closeFile(file));

	return true;
}