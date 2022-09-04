// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteHarmonizer.h"
#include "NoteHarmonizer_cpp.h"

// Sets default values for this component's properties
UNoteHarmonizer::UNoteHarmonizer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	lastChords_ = { -1 };
}


// Called when the game starts
void UNoteHarmonizer::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

TArray<int> UNoteHarmonizer::GetAppropriateChordFromPitch(int note, bool major, int keyCenter)
{
	std::vector<std::vector<std::vector<int>>> stack;
	if (major) {
		stack = NoteHarmonizer::getMajorChordStack(keyCenter);
	}
	else {
		stack = NoteHarmonizer::getMinorChordStack(keyCenter);
	}
	lastChords_ = NoteHarmonizer::getAppropriateChordFromPitch(note, stack, lastChords_);

	TArray<int> returnedChords;
	for (int chord : lastChords_) {
		returnedChords.Add(chord);
	}
	return returnedChords;
}