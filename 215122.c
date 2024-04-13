Status RemapVectorToMap(const TTypes<const int64>::Vec& remapping,
                        std::vector<bool>* id_present,
                        std::unordered_map<int64, int64>* old_id_to_new_id) {
  id_present->clear();
  id_present->resize(remapping.size(), false);
  for (int i = 0; i < remapping.size(); ++i) {
    const int64 old_id = remapping(i);
    if (old_id < 0) continue;
    (*id_present)[i] = true;
    if (!gtl::InsertIfNotPresent(old_id_to_new_id, old_id, i)) {
      return errors::Unimplemented(
          strings::StrCat("Old ID ", old_id, " is mapped to both new ID ",
                          old_id_to_new_id->at(old_id), " and ", i,
                          ", which is not supported."));
    }
  }
  return Status::OK();
}