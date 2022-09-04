#pragma once
#include <vector>
class NoteHarmonizer {
public:
	static std::vector<std::vector<std::vector<int>>> getMajorChordStack(int keyCenter);
	static std::vector<std::vector<std::vector<int>>> getMinorChordStack(int keyCenter);
	static std::vector<std::vector<std::vector<int>>> getChordStack(int keyCenter, std::vector<std::vector<std::vector<int>>> chordStackConfig);
	static std::vector<int> getAppropriateChordFromPitch(int pitch, std::vector<std::vector<std::vector<int>>> chordStack, std::vector<int> lastChord);

private:
	static std::vector<int> getAppropriateChordFromNote(int pitch, std::vector<std::vector<std::vector<int>>> chordStack, std::vector<int> lastChord);
	static std::vector<int> getChordWithMinimalLayerDifference(int note, std::vector<std::vector<std::vector<int>>> chordStack, int layer, bool ignoreSeventh);
	static std::vector<int> getChordWithMinimalLayerNumber(int note, std::vector<std::vector<std::vector<int>>> chordStack, int minLayer, bool ignoreSeventh);
	static int getChordIndexInLayerWithNote(int note, int layerIndex, std::vector<std::vector<std::vector<int>>> chordStack, bool ignoreSeventh);
	static int getChordLayer(std::vector<int> chord, std::vector<std::vector<std::vector<int>>> chordStack);
	static std::vector<int> makeChord(std::vector<int> chordTemplate, int key);
	static std::vector<std::vector<int>> makeChordLayer(int key, std::vector<std::vector<int>> templates);
};