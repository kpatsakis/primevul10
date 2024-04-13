CallResult<Handle<JSArray>> JSObject::getOwnPropertyKeys(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    OwnKeysFlags okFlags) {
  assert(
      (okFlags.getIncludeNonSymbols() || okFlags.getIncludeSymbols()) &&
      "Can't exclude symbols and strings");
  if (LLVM_UNLIKELY(
          selfHandle->flags_.lazyObject || selfHandle->flags_.proxyObject)) {
    if (selfHandle->flags_.proxyObject) {
      CallResult<PseudoHandle<JSArray>> proxyRes =
          JSProxy::ownPropertyKeys(selfHandle, runtime, okFlags);
      if (LLVM_UNLIKELY(proxyRes == ExecutionStatus::EXCEPTION)) {
        return ExecutionStatus::EXCEPTION;
      }
      return runtime->makeHandle(std::move(*proxyRes));
    }
    assert(selfHandle->flags_.lazyObject && "descriptor flags are impossible");
    initializeLazyObject(runtime, selfHandle);
  }

  auto range = getOwnIndexedRange(selfHandle.get(), runtime);

  // Estimate the capacity of the output array.  This estimate is only
  // reasonable for the non-symbol case.
  uint32_t capacity = okFlags.getIncludeNonSymbols()
      ? (selfHandle->clazz_.get(runtime)->getNumProperties() + range.second -
         range.first)
      : 0;

  auto arrayRes = JSArray::create(runtime, capacity, 0);
  if (LLVM_UNLIKELY(arrayRes == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  auto array = runtime->makeHandle(std::move(*arrayRes));

  // Optional array of SymbolIDs reported via host object API
  llvh::Optional<Handle<JSArray>> hostObjectSymbols;
  size_t hostObjectSymbolCount = 0;

  // If current object is a host object we need to deduplicate its properties
  llvh::SmallSet<SymbolID::RawType, 16> dedupSet;

  // Output index.
  uint32_t index = 0;

  // Avoid allocating a new handle per element.
  MutableHandle<> tmpHandle{runtime};

  // Number of indexed properties.
  uint32_t numIndexed = 0;

  // Regular properties with names that are array indexes are stashed here, if
  // encountered.
  llvh::SmallVector<uint32_t, 8> indexNames{};

  // Iterate the named properties excluding those which use Symbols.
  if (okFlags.getIncludeNonSymbols()) {
    // Get host object property names
    if (LLVM_UNLIKELY(selfHandle->flags_.hostObject)) {
      assert(
          range.first == range.second &&
          "Host objects cannot own indexed range");
      auto hostSymbolsRes =
          vmcast<HostObject>(selfHandle.get())->getHostPropertyNames();
      if (hostSymbolsRes == ExecutionStatus::EXCEPTION) {
        return ExecutionStatus::EXCEPTION;
      }
      if ((hostObjectSymbolCount = (**hostSymbolsRes)->getEndIndex()) != 0) {
        Handle<JSArray> hostSymbols = *hostSymbolsRes;
        hostObjectSymbols = std::move(hostSymbols);
        capacity += hostObjectSymbolCount;
      }
    }

    // Iterate the indexed properties.
    GCScopeMarkerRAII marker{runtime};
    for (auto i = range.first; i != range.second; ++i) {
      auto res = getOwnIndexedPropertyFlags(selfHandle.get(), runtime, i);
      if (!res)
        continue;

      // If specified, check whether it is enumerable.
      if (!okFlags.getIncludeNonEnumerable() && !res->enumerable)
        continue;

      tmpHandle = HermesValue::encodeDoubleValue(i);
      JSArray::setElementAt(array, runtime, index++, tmpHandle);
      marker.flush();
    }

    numIndexed = index;

    HiddenClass::forEachProperty(
        runtime->makeHandle(selfHandle->clazz_),
        runtime,
        [runtime,
         okFlags,
         array,
         hostObjectSymbolCount,
         &index,
         &indexNames,
         &tmpHandle,
         &dedupSet](SymbolID id, NamedPropertyDescriptor desc) {
          if (!isPropertyNamePrimitive(id)) {
            return;
          }

          // If specified, check whether it is enumerable.
          if (!okFlags.getIncludeNonEnumerable()) {
            if (!desc.flags.enumerable)
              return;
          }

          // Host properties might overlap with the ones recognized by the
          // hidden class. If we're dealing with a host object then keep track
          // of hidden class properties for the deduplication purposes.
          if (LLVM_UNLIKELY(hostObjectSymbolCount > 0)) {
            dedupSet.insert(id.unsafeGetRaw());
          }

          // Check if this property is an integer index. If it is, we stash it
          // away to deal with it later. This check should be fast since most
          // property names don't start with a digit.
          auto propNameAsIndex = toArrayIndex(
              runtime->getIdentifierTable().getStringView(runtime, id));
          if (LLVM_UNLIKELY(propNameAsIndex)) {
            indexNames.push_back(*propNameAsIndex);
            return;
          }

          tmpHandle = HermesValue::encodeStringValue(
              runtime->getStringPrimFromSymbolID(id));
          JSArray::setElementAt(array, runtime, index++, tmpHandle);
        });

    // Iterate over HostObject properties and append them to the array. Do not
    // append duplicates.
    if (LLVM_UNLIKELY(hostObjectSymbols)) {
      for (size_t i = 0; i < hostObjectSymbolCount; ++i) {
        assert(
            (*hostObjectSymbols)->at(runtime, i).isSymbol() &&
            "Host object needs to return array of SymbolIDs");
        marker.flush();
        SymbolID id = (*hostObjectSymbols)->at(runtime, i).getSymbol();
        if (dedupSet.count(id.unsafeGetRaw()) == 0) {
          dedupSet.insert(id.unsafeGetRaw());

          assert(
              !InternalProperty::isInternal(id) &&
              "host object returned reserved symbol");
          auto propNameAsIndex = toArrayIndex(
              runtime->getIdentifierTable().getStringView(runtime, id));
          if (LLVM_UNLIKELY(propNameAsIndex)) {
            indexNames.push_back(*propNameAsIndex);
            continue;
          }
          tmpHandle = HermesValue::encodeStringValue(
              runtime->getStringPrimFromSymbolID(id));
          JSArray::setElementAt(array, runtime, index++, tmpHandle);
        }
      }
    }
  }

  // Now iterate the named properties again, including only Symbols.
  // We could iterate only once, if we chose to ignore (and disallow)
  // own properties on HostObjects, as we do with Proxies.
  if (okFlags.getIncludeSymbols()) {
    MutableHandle<SymbolID> idHandle{runtime};
    HiddenClass::forEachProperty(
        runtime->makeHandle(selfHandle->clazz_),
        runtime,
        [runtime, okFlags, array, &index, &idHandle](
            SymbolID id, NamedPropertyDescriptor desc) {
          if (!isSymbolPrimitive(id)) {
            return;
          }
          // If specified, check whether it is enumerable.
          if (!okFlags.getIncludeNonEnumerable()) {
            if (!desc.flags.enumerable)
              return;
          }
          idHandle = id;
          JSArray::setElementAt(array, runtime, index++, idHandle);
        });
  }

  // The end (exclusive) of the named properties.
  uint32_t endNamed = index;

  // Properly set the length of the array.
  auto cr = JSArray::setLength(
      array, runtime, endNamed + indexNames.size(), PropOpFlags{});
  (void)cr;
  assert(
      cr != ExecutionStatus::EXCEPTION && *cr && "JSArray::setLength() failed");

  // If we have no index-like names, we are done.
  if (LLVM_LIKELY(indexNames.empty()))
    return array;

  // In the unlikely event that we encountered index-like names, we need to sort
  // them and merge them with the real indexed properties. Note that it is
  // guaranteed that there are no clashes.
  std::sort(indexNames.begin(), indexNames.end());

  // Also make space for the new elements by shifting all the named properties
  // to the right. First, resize the array.
  JSArray::setStorageEndIndex(array, runtime, endNamed + indexNames.size());

  // Shift the non-index property names. The region [numIndexed..endNamed) is
  // moved to [numIndexed+indexNames.size()..array->size()).
  // TODO: optimize this by implementing memcpy-like functionality in ArrayImpl.
  for (uint32_t last = endNamed, toLast = array->getEndIndex();
       last != numIndexed;) {
    --last;
    --toLast;
    tmpHandle = array->at(runtime, last);
    JSArray::setElementAt(array, runtime, toLast, tmpHandle);
  }

  // Now we need to merge the indexes in indexNames and the array
  // [0..numIndexed). We start from the end and copy the larger element from
  // either array.
  // 1+ the destination position to copy into.
  for (uint32_t toLast = numIndexed + indexNames.size(),
                indexNamesLast = indexNames.size();
       toLast != 0;) {
    if (numIndexed) {
      uint32_t a = (uint32_t)array->at(runtime, numIndexed - 1).getNumber();
      uint32_t b;

      if (indexNamesLast && (b = indexNames[indexNamesLast - 1]) > a) {
        tmpHandle = HermesValue::encodeDoubleValue(b);
        --indexNamesLast;
      } else {
        tmpHandle = HermesValue::encodeDoubleValue(a);
        --numIndexed;
      }
    } else {
      assert(indexNamesLast && "prematurely ran out of source values");
      tmpHandle =
          HermesValue::encodeDoubleValue(indexNames[indexNamesLast - 1]);
      --indexNamesLast;
    }

    --toLast;
    JSArray::setElementAt(array, runtime, toLast, tmpHandle);
  }

  return array;
}