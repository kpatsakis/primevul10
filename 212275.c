bool CPlayListASX::LoadAsxIniInfo(std::istream &stream)
{
  CLog::Log(LOGINFO, "Parsing INI style ASX");

  std::string name, value;

  while( stream.good() )
  {
    // consume blank rows, and blanks
    while((stream.peek() == '\r' || stream.peek() == '\n' || stream.peek() == ' ') && stream.good())
      stream.get();

    if(stream.peek() == '[')
    {
      // this is an [section] part, just ignore it
      while(stream.good() && stream.peek() != '\r' && stream.peek() != '\n')
        stream.get();
      continue;
    }
    name = "";
    value = "";
    // consume name
    while(stream.peek() != '\r' && stream.peek() != '\n' && stream.peek() != '=' && stream.good())
      name += stream.get();

    // consume =
    if(stream.get() != '=')
      continue;

    // consume value
    while(stream.peek() != '\r' && stream.peek() != '\n' && stream.good())
      value += stream.get();

    CLog::Log(LOGINFO, "Adding element {}={}", name, value);
    CFileItemPtr newItem(new CFileItem(value));
    newItem->SetPath(value);
    if (newItem->IsVideo() && !newItem->HasVideoInfoTag()) // File is a video and needs a VideoInfoTag
      newItem->GetVideoInfoTag()->Reset(); // Force VideoInfoTag creation
    Add(newItem);
  }

  return true;
}