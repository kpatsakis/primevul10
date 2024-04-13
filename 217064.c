  GroupIterable group(const VarDimArray& group_ix) const {
    DCHECK_LE(group_ix.size(), dims_);
    for (std::size_t di = 0; di < group_ix.size(); ++di) {
      DCHECK_GE(group_ix[di], 0) << "Group dimension out of range";
      DCHECK_LT(group_ix[di], dims_) << "Group dimension out of range";
      DCHECK_EQ(group_ix[di], order_[di])
          << "Group dimension does not match sorted order";
    }
    return GroupIterable(ix_, vals_, dims_, group_ix);
  }