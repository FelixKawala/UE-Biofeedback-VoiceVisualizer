// Fill out your copyright notice in the Description page of Project Settings.

#include "FileWriter.h"

const FString UFileWriter::ParticipantNumberFilePath_ =
	FPaths::ProjectDir().Append(TEXT("Messure/")).Append(TEXT("CurrentParticipant.txt"));
FString UFileWriter::participantString_ = TEXT("");
FString UFileWriter::conditionString_ = TEXT("");
uint32 UFileWriter::participantNumber_ = 0;

uint32 UFileWriter::GetCurrentParticipantNumber()
{
	// Get current participant number.
	FString participantNumberFileString;
	if (FFileHelper::LoadFileToString(participantNumberFileString, *ParticipantNumberFilePath_))
	{
		// Could load.
		return FCString::Atoi(*participantNumberFileString);
	}
	// Could not load.
	return 0;
}
bool UFileWriter::AdvanceCurrentParticipantNumber()
{
	return FFileHelper::SaveStringToFile(FString::FromInt(++participantNumber_), *ParticipantNumberFilePath_);
}

FString BuildDateTimeString()
{
	// Get current time.
	int32 second, minute, hour, day, month, year;
	const FDateTime now = FDateTime::Now();
	now.GetDate(year, month, day);
	hour = now.GetHour();
	minute = now.GetMinute();
	second = now.GetSecond();

	return FString::Printf(TEXT("%d-%d-%d_%d-%d-%d"), year, month, day, hour, minute, second);
}

FString BuildParticipantString(int64 participantNumber)
{
	return FString::Printf(TEXT("PT-%d_"), participantNumber).Append(BuildDateTimeString());
}

FString UFileWriter::BuildPath()
{
	FString path = FPaths::ProjectDir().Append(TEXT("Messure/"));
	path.Append(participantString_.IsEmpty() ? BuildParticipantString(-666) : participantString_).Append("/");
	return path;
}

bool UFileWriter::NewParticipant()
{
	if (participantNumber_ == 0)
	{
		participantNumber_ = GetCurrentParticipantNumber();
	}
	participantString_ = BuildParticipantString(participantNumber_);
	return AdvanceCurrentParticipantNumber();
}

void UFileWriter::NewCondition(FString condition)
{
	conditionString_ = condition;
}

bool UFileWriter::WriteStrings(UPARAM(ref) TArray<FString>& strings, FString fileName)
{
	return FFileHelper::SaveStringArrayToFile(strings, *(BuildPath().Append(conditionString_.IsEmpty() ? TEXT("") : conditionString_.Append("_")).Append(fileName)));
}

bool UFileWriter::WriteStringsToCSV(UPARAM(ref) TArray<FString>& strings)
{
	// Append Participant and Condition for CSV.
	if (!strings.IsEmpty())
	{
		strings[0].Append(TEXT(", Participant, Condition"));
		for (size_t i = 1; i < strings.Num(); ++i)
		{
			strings[i].Append(TEXT(", ")).Append(participantString_.IsEmpty() ? TEXT("Unknown") : FString::FromInt(participantNumber_));
			strings[i].Append(TEXT(", ")).Append(conditionString_.IsEmpty() ? TEXT("Unknown") : conditionString_);
		}
	}

	return WriteStrings(strings, TEXT("Done.csv"));
}
