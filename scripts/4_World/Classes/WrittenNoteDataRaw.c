//! Vanilla WrittenNoteData.SetNoteText() runs MiscGameplayFunctions.SanitizeString(), which cuts
//! the note at 512 BYTES - for Cyrillic text (2 bytes per character) that is only ~250 letters,
//! so long notes were silently truncated. The storage field itself has no such limit, only the
//! sanitizer does, so the mod stores the text verbatim through this raw setter.
modded class WrittenNoteData
{
	void SetNoteTextRaw(string text)
	{
		m_SimpleText = text;
	}
}
