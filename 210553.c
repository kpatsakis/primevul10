SpeechSynthesis::SpeechSynthesis(ExecutionContext* context)
    : ContextLifecycleObserver(context)
    , m_platformSpeechSynthesizer(PlatformSpeechSynthesizer::create(this))
    , m_isPaused(false)
{
    ScriptWrappable::init(this);
}
