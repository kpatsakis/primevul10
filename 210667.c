ScriptPromise AudioContext::suspendContext(ScriptState* scriptState)
{
    ASSERT(isMainThread());
    AutoLocker locker(this);

    if (isOfflineContext()) {
        return ScriptPromise::rejectWithDOMException(
            scriptState,
            DOMException::create(
                InvalidStateError,
                "cannot suspend an OfflineAudioContext"));
    }

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    m_suspendResolvers.append(resolver);

    return promise;
}
