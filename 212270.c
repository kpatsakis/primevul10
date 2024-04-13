bool CPlayListPLS::Load(const std::string &strFile)
{
  //read it from the file
  std::string strFileName(strFile);
  m_strPlayListName = URIUtils::GetFileName(strFileName);

  Clear();

  bool bShoutCast = false;
  if( StringUtils::StartsWithNoCase(strFileName, "shout://") )
  {
    strFileName.replace(0, 8, "http://");
    m_strBasePath = "";
    bShoutCast = true;
  }
  else
    URIUtils::GetParentPath(strFileName, m_strBasePath);

  CFile file;
  if (!file.Open(strFileName) )
  {
    file.Close();
    return false;
  }

  if (file.GetLength() > 1024*1024)
  {
    CLog::Log(LOGWARNING, "{} - File is larger than 1 MB, most likely not a playlist",
              __FUNCTION__);
    return false;
  }

  char szLine[4096];
  std::string strLine;

  // run through looking for the [playlist] marker.
  // if we find another http stream, then load it.
  while (true)
  {
    if ( !file.ReadString(szLine, sizeof(szLine) ) )
    {
      file.Close();
      return size() > 0;
    }
    strLine = szLine;
    StringUtils::Trim(strLine);
    if(StringUtils::EqualsNoCase(strLine, START_PLAYLIST_MARKER))
      break;

    // if there is something else before playlist marker, this isn't a pls file
    if(!strLine.empty())
      return false;
  }

  bool bFailed = false;
  while (file.ReadString(szLine, sizeof(szLine) ) )
  {
    strLine = szLine;
    StringUtils::RemoveCRLF(strLine);
    size_t iPosEqual = strLine.find('=');
    if (iPosEqual != std::string::npos)
    {
      std::string strLeft = strLine.substr(0, iPosEqual);
      iPosEqual++;
      std::string strValue = strLine.substr(iPosEqual);
      StringUtils::ToLower(strLeft);
      StringUtils::TrimLeft(strLeft);

      if (strLeft == "numberofentries")
      {
        m_vecItems.reserve(atoi(strValue.c_str()));
      }
      else if (StringUtils::StartsWith(strLeft, "file"))
      {
        std::vector <int>::size_type idx = atoi(strLeft.c_str() + 4);
        if (!Resize(idx))
        {
          bFailed = true;
          break;
        }

        // Skip self - do not load playlist recursively
        if (StringUtils::EqualsNoCase(URIUtils::GetFileName(strValue),
                                      URIUtils::GetFileName(strFileName)))
          continue;

        if (m_vecItems[idx - 1]->GetLabel().empty())
          m_vecItems[idx - 1]->SetLabel(URIUtils::GetFileName(strValue));
        CFileItem item(strValue, false);
        if (bShoutCast && !item.IsAudio())
          strValue.replace(0, 7, "shout://");

        strValue = URIUtils::SubstitutePath(strValue);
        CUtil::GetQualifiedFilename(m_strBasePath, strValue);
        g_charsetConverter.unknownToUTF8(strValue);
        m_vecItems[idx - 1]->SetPath(strValue);
      }
      else if (StringUtils::StartsWith(strLeft, "title"))
      {
        std::vector <int>::size_type idx = atoi(strLeft.c_str() + 5);
        if (!Resize(idx))
        {
          bFailed = true;
          break;
        }
        g_charsetConverter.unknownToUTF8(strValue);
        m_vecItems[idx - 1]->SetLabel(strValue);
      }
      else if (StringUtils::StartsWith(strLeft, "length"))
      {
        std::vector <int>::size_type idx = atoi(strLeft.c_str() + 6);
        if (!Resize(idx))
        {
          bFailed = true;
          break;
        }
        m_vecItems[idx - 1]->GetMusicInfoTag()->SetDuration(atol(strValue.c_str()));
      }
      else if (strLeft == "playlistname")
      {
        m_strPlayListName = strValue;
        g_charsetConverter.unknownToUTF8(m_strPlayListName);
      }
    }
  }
  file.Close();

  if (bFailed)
  {
    CLog::Log(LOGERROR,
              "File {} is not a valid PLS playlist. Location of first file,title or length is not "
              "permitted (eg. File0 should be File1)",
              URIUtils::GetFileName(strFileName));
    return false;
  }

  // check for missing entries
  ivecItems p = m_vecItems.begin();
  while ( p != m_vecItems.end())
  {
    if ((*p)->GetPath().empty())
    {
      p = m_vecItems.erase(p);
    }
    else
    {
      ++p;
    }
  }

  return true;
}