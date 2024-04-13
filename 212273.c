void CPlayListPLS::Save(const std::string& strFileName) const
{
  if (!m_vecItems.size()) return ;
  std::string strPlaylist = CUtil::MakeLegalPath(strFileName);
  CFile file;
  if (!file.OpenForWrite(strPlaylist, true))
  {
    CLog::Log(LOGERROR, "Could not save PLS playlist: [{}]", strPlaylist);
    return;
  }
  std::string write;
  write += StringUtils::Format("{}\n", START_PLAYLIST_MARKER);
  std::string strPlayListName=m_strPlayListName;
  g_charsetConverter.utf8ToStringCharset(strPlayListName);
  write += StringUtils::Format("PlaylistName={}\n", strPlayListName);

  for (int i = 0; i < (int)m_vecItems.size(); ++i)
  {
    CFileItemPtr item = m_vecItems[i];
    std::string strFileName=item->GetPath();
    g_charsetConverter.utf8ToStringCharset(strFileName);
    std::string strDescription=item->GetLabel();
    g_charsetConverter.utf8ToStringCharset(strDescription);
    write += StringUtils::Format("File{}={}\n", i + 1, strFileName);
    write += StringUtils::Format("Title{}={}\n", i + 1, strDescription.c_str());
    write +=
        StringUtils::Format("Length{}={}\n", i + 1, item->GetMusicInfoTag()->GetDuration() / 1000);
  }

  write += StringUtils::Format("NumberOfEntries={0}\n", m_vecItems.size());
  write += StringUtils::Format("Version=2\n");
  file.Write(write.c_str(), write.size());
  file.Close();
}