bool CPlayListRAM::LoadData(std::istream& stream)
{
  CLog::Log(LOGINFO, "Parsing RAM");

  std::string strMMS;
  while( stream.peek() != '\n' && stream.peek() != '\r' )
    strMMS += stream.get();

  CLog::Log(LOGINFO, "Adding element {}", strMMS);
  CFileItemPtr newItem(new CFileItem(strMMS));
  newItem->SetPath(strMMS);
  Add(newItem);
  return true;
}