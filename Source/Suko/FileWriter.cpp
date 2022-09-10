// Fill out your copyright notice in the Description page of Project Settings.

#include "FileWriter.h"

const FString UFileWriter::ParticipantNumberFilePath_ =
	FPaths::ProjectDir().Append(TEXT("Messure/")).Append(TEXT("CurrentParticipant.txt"));
FString UFileWriter::participantString_ = TEXT("");
FString UFileWriter::conditionString_ = TEXT("");
uint32 UFileWriter::participantNumber_ = 0;

TArray<FString> UFileWriter::fileContent_ = {};

uint32 UFileWriter::GetCurrentParticipantNumber()
{
	// Get current participant number.
	FString participantNumberFileString;
	if (FFileHelper::LoadFileToString(participantNumberFileString, *ParticipantNumberFilePath_)){
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
	if (participantString_.IsEmpty()){
		participantString_ = BuildParticipantString(GetCurrentParticipantNumber());
	}
	path.Append(participantString_).Append("/");
	return path;
}

bool UFileWriter::NewParticipant(int& participantNumber)
{
	if (participantNumber_ == 0){
		participantNumber_ = GetCurrentParticipantNumber();
	}
	const bool ret = AdvanceCurrentParticipantNumber();
	participantString_ = BuildParticipantString(participantNumber_);
	participantNumber = participantNumber_;
	return ret;
}

void UFileWriter::NewCondition(FString condition)
{
	conditionString_ = condition;
}

void UFileWriter::AddContent(FString row)
{
	fileContent_.Push(row);
}
TArray<FString>& UFileWriter::GetContent()
{
	return fileContent_;
}
void UFileWriter::WriteContent(FString fileName)
{
	if (fileContent_.IsEmpty()){
		return;
	}
	WriteStrings(fileContent_, fileName);
	fileContent_.Empty();
}
void UFileWriter::WriteContentToCSV()
{
	if (fileContent_.IsEmpty()){
		return;
	}
	WriteStringsToCSV(fileContent_);
	fileContent_.Empty();
}

bool UFileWriter::WriteStrings(UPARAM(ref) TArray<FString>& strings, FString fileName)
{
	// Find unused file name.
	const FString fileNameClean = FPaths::GetBaseFilename(fileName);
	const FString fileNameExtension = FPaths::GetExtension(fileName, true);
	const FString filePath =
		BuildPath().Append(conditionString_.IsEmpty() ? TEXT("") : conditionString_ + TEXT("_")).Append(fileNameClean);
	FString writeFilePath = filePath + fileNameExtension;
	int fileCount = 0;
	while (FPaths::FileExists(writeFilePath)){
		writeFilePath = filePath + FString::FromInt(++fileCount) + fileNameExtension;
	}
	// Write file.
	return FFileHelper::SaveStringArrayToFile(strings, *(writeFilePath));
}

bool UFileWriter::WriteStringsToCSV(TArray<FString> strings)
{
	// Append Participant and Condition for CSV.
	if (!strings.IsEmpty())
	{
		if (participantString_.IsEmpty()){
			participantNumber_ = GetCurrentParticipantNumber();
		}
		strings[0].Append(TEXT(", Participant, Condition"));
		for (size_t i = 1; i < strings.Num(); ++i){
			strings[i].Append(TEXT(", ")).Append(FString::FromInt(participantNumber_));
			strings[i].Append(TEXT(", ")).Append(conditionString_.IsEmpty() ? TEXT("Unknown") : conditionString_);
		}
	}

	return WriteStrings(strings, TEXT("Done.csv"));
}
