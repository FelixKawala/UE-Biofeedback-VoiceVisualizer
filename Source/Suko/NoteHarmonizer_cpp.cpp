#include "NoteHarmonizer_cpp.h"

std::vector<int> MAJ_7_CH = {0, 4, 7, 10};
std::vector<int> MAJ_M7_CH = {0, 4, 7, 11};
std::vector<int> MIN_7_CH = {0, 3, 7, 10};
std::vector<int> MIN_7_b5_CH = { 0, 3, 6, 10 };
std::vector<int> DIM_CH = {0, 3, 6, 9};

std::vector<std::vector<std::vector<int>>> MAJOR_STACK_CONFIG = {
    {MAJ_M7_CH},                    //L0
    {MAJ_7_CH},                     //L1
    {MIN_7_CH, MAJ_7_CH},           //L2
    {MIN_7_CH, MAJ_7_CH},           //L3
    {MIN_7_CH, MAJ_7_CH},           //L4
    {MIN_7_b5_CH},                  //L5
    {MIN_7_CH},                     //L6
    {MAJ_M7_CH},                    //L7(-5)
    {MAJ_M7_CH},                    //L8(-4)
    {MAJ_M7_CH},                    //L9(-3)
    {MAJ_M7_CH},                    //L10(-2)
    {MAJ_M7_CH}                     //L11(-1)
};

std::vector<std::vector<std::vector<int>>> MINOR_STACK_CONFIG = {
    {MIN_7_CH},                     //L0
    {MAJ_7_CH, MIN_7_CH},           //L1
    {MIN_7_b5_CH},                  //L2
    {MIN_7_b5_CH},                  //L3
    {MIN_7_b5_CH},                  //L4
    {DIM_CH},                       //L5
    { MAJ_M7_CH },                  //L6
    { MAJ_M7_CH },                  //L7(-5)
    { MAJ_M7_CH },                  //L8(-4)
    { MAJ_M7_CH },                  //L9(-3)
    { MAJ_7_CH },                   //L10(-2)
    { MIN_7_CH }                  //L11(-1)
};


std::vector<std::vector<std::vector<int>>> NoteHarmonizer::getMajorChordStack(int keyCenter) {
    return getChordStack(keyCenter, MAJOR_STACK_CONFIG);
}

std::vector<std::vector<std::vector<int>>> NoteHarmonizer::getMinorChordStack(int keyCenter) {
    return getChordStack(keyCenter, MINOR_STACK_CONFIG);
}

std::vector<std::vector<std::vector<int>>> NoteHarmonizer::getChordStack(int keyCenter, std::vector<std::vector<std::vector<int>>> chordStackConfig) {
    std::vector<std::vector<std::vector<int>>> chordStack;
    int key = keyCenter;
    for (int i = 0; i < 12; ++i) {
        chordStack.push_back(makeChordLayer(key, chordStackConfig.at(i)));
        key = (key + 7) % 12;
    }
    return chordStack;
}

std::vector<int> NoteHarmonizer::getAppropriateChordFromPitch(int pitch, std::vector<std::vector<std::vector<int>>> chordStack, std::vector<int> lastChord) {
    int note = pitch % 12;
    std::vector<int> lastChordNotes(lastChord);
    if (lastChordNotes.at(0) > -1) {
        for (int i = 0; i < lastChordNotes.size(); ++i) {
            lastChordNotes.at(i) = lastChordNotes.at(i) % 12;
        }
    }
    std::vector<int> chordPitches = getAppropriateChordFromNote(note, chordStack, lastChordNotes);
    if (chordPitches.at(0) != -1) {
        for (int i = 0; i < chordPitches.size(); ++i) {

            while (chordPitches.at(i) < pitch - 12)
                chordPitches.at(i) += 12;
        }
    }
    return chordPitches;
}

std::vector<int> NoteHarmonizer::getAppropriateChordFromNote(int note, std::vector<std::vector<std::vector<int>>> chordStack, std::vector<int> lastChord) {
    // First chord : the chord is whatever chord the note is in with minimal layer number
    if (lastChord.at(0) == -1) {
        return getChordWithMinimalLayerNumber(note, chordStack, 0, true);
    }
        
    // Last chord is at layer 0: do the same thing as previous case, except landing again on layer 0
    int layerLastChord = getChordLayer(lastChord, chordStack);
    if (layerLastChord == 0) {
        return getChordWithMinimalLayerNumber(note, chordStack, 1, true);
    }
    // If layer is 1 or 11: check if if could go back to 0
    else if (layerLastChord == 1 || layerLastChord == 11) {
        // If not clashing with chord layer 0: force to layer zero
        bool clash = false;
        int chordLength = chordStack.at(0).at(0).size();
        for (int i = 0; i < std::min(3,chordLength); ++i) {
            int diff = std::abs(note - chordStack.at(0).at(0).at(i));
            if (diff == 0) {
                clash = false;
                break;
            }
            else if (diff == 1) {
                clash = true;
            }
        }
        if (clash == false)
            return chordStack.at(0).at(0);
        // Otherwise searching elsewhere
        else
            return getChordWithMinimalLayerNumber(note, chordStack, 1, true);
    }
    // Otherwise : search for a chord with the least difference in layer number.
    else
        return getChordWithMinimalLayerDifference(note, chordStack, layerLastChord, true);
}

std::vector<int> NoteHarmonizer::getChordWithMinimalLayerDifference(int note, std::vector<std::vector<std::vector<int>>> chordStack, int layer, bool ignoreSeventh) {
    for (int i = 0; i < 12; ++i) {
        int upperLayer = (layer - i) % 12;
        int lowerLayer = (layer + i) % 12;
        int indexUpper = getChordIndexInLayerWithNote(note, upperLayer, chordStack, ignoreSeventh);
        int indexLower = getChordIndexInLayerWithNote(note, lowerLayer, chordStack, ignoreSeventh);
        if (indexUpper > -1)
            return chordStack.at(upperLayer).at(indexUpper);
        else if (indexLower > -1)
            return chordStack.at(lowerLayer).at(indexLower);
    }
    std::vector<int> emptyChord = {-1};
    return emptyChord;
}

std::vector<int> NoteHarmonizer::getChordWithMinimalLayerNumber(int note, std::vector<std::vector<std::vector<int>>> chordStack, int minLayer, bool ignoreSeventh) {
    for (int i = 0; i <= 6; ++i) {
        int indexRight = getChordIndexInLayerWithNote(note, (i) % 12, chordStack, ignoreSeventh);
        int indexLeft = getChordIndexInLayerWithNote(note, (12 - i) % 12, chordStack, ignoreSeventh);
        if (indexRight > -1)
            return chordStack.at((i) % 12).at(indexRight);
        else if (indexLeft > -1)
            return chordStack.at((12 - i) % 12).at(indexLeft);
    }
    std::vector<int> emptyChord = { -1 };
    return emptyChord;
}

int NoteHarmonizer::getChordIndexInLayerWithNote(int note, int layerIndex, std::vector<std::vector<std::vector<int>>> chordStack, bool ignoreSeventh) {
    for (int i = 0; i < chordStack.at(layerIndex).size(); ++i) {
        int s = chordStack.at(layerIndex).at(i).size();
        if (ignoreSeventh) s = std::min(3, s);
        for (int k = 0; k < s; ++k) {
            if (note == chordStack.at(layerIndex).at(i).at(k)) {
                return i;
            }
        }
    }
    return -1;
}

int NoteHarmonizer::getChordLayer(std::vector<int> chord, std::vector<std::vector<std::vector<int>>> chordStack) {
    for (int i = 0; i < 12; ++i) {
        for (int j = 0; j < chordStack.at(i).size(); ++j) {
            if (chord.at(0) == chordStack.at(i).at(j).at(0))
                return i;
        }
    }
    return -1;
}

std::vector<int> NoteHarmonizer::makeChord(std::vector<int> chordTemplate, int key) {
    std::vector<int> t(chordTemplate);
    for (int i = 0; i < t.size(); ++i)
        t.at(i) = (t.at(i) + key) % 12;
    return t;
}

std::vector<std::vector<int>> NoteHarmonizer::makeChordLayer(int key, std::vector<std::vector<int>> layerTemplates) {
    std::vector<std::vector<int>> layer;
    for (int i = 0; i < layerTemplates.size(); ++i)
        layer.push_back(makeChord(layerTemplates.at(i), key));
    return layer;
}