bool CPlayListPLS::Resize(std::vector <int>::size_type newSize)
{
  if (newSize == 0)
    return false;

  while (m_vecItems.size() < newSize)
  {
    CFileItemPtr fileItem(new CFileItem());
    m_vecItems.push_back(fileItem);
  }
  return true;
}