PassRefPtrWillBeRawPtr<SpeechSynthesis> SpeechSynthesis::create(ExecutionContext* context)
{
    return adoptRefWillBeRefCountedGarbageCollected(new SpeechSynthesis(context));
}
