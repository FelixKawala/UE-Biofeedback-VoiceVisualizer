// Fill out your copyright notice in the Description page of Project Settings.

#include "VoicePitchDisplay.h"

#include "RESTfunctionLibraray.h"

// Sets default values
AVoicePitchDisplay::AVoicePitchDisplay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Add static mesh components to actor
	m_DisplayBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	m_DisplayHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hand"));
	m_DisplayTarget = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Target"));
	m_DisplayNextTarget = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Next Target"));
	m_DisplayTargetInstanced =
		CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName(m_DisplayTarget->GetName().Append(TEXT(" Instance"))));
	m_DisplayNextTargetInstanced =
		CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName(m_DisplayNextTarget->GetName().Append(TEXT(" Instance"))));

	SetRootComponent(m_DisplayBase);
	m_DisplayHand->SetupAttachment(m_DisplayBase);
	m_DisplayTarget->SetupAttachment(m_DisplayBase);
	m_DisplayNextTarget->SetupAttachment(m_DisplayBase);
	m_DisplayTargetInstanced->SetupAttachment(m_DisplayBase);
	m_DisplayNextTargetInstanced->SetupAttachment(m_DisplayBase);

	m_DisplayTarget->SetCastShadow(false);
	m_DisplayNextTarget->SetCastShadow(false);
}

// Called when the game starts or when spawned
void AVoicePitchDisplay::BeginPlay()
{
	Super::BeginPlay();

	// Setup instanced static mesh component.
	StaticMeshToInstance(m_DisplayTarget, m_DisplayTargetInstanced);
	StaticMeshToInstance(m_DisplayNextTarget, m_DisplayNextTargetInstanced);

	// Add instances of static mesh.
	const float rotationPerTarget = 360.f / m_TargetAmount;

	const FTransform targetTransform = m_DisplayTargetInstanced->GetRelativeTransform();
	TArray<FTransform> targetTransformArr;
	targetTransformArr.SetNum(m_TargetAmount);
	for (int i = 0; i < m_TargetAmount; ++i)
	{
		targetTransformArr[i] = targetTransform;
		targetTransformArr[i].SetRotation(FQuat(targetTransformArr[i].Rotator().Add(180, rotationPerTarget * i, 0)));
	}
	const FTransformArrayA2 targetTransformArray(targetTransformArr, m_TargetAmount);
	m_DisplayTargetInstanced->AddInstances(targetTransformArray, false, false);
	m_DisplayTargetInstanced->SetCastShadow(false);

	const FTransform nextTargetTransform = m_DisplayNextTargetInstanced->GetComponentTransform();
	TArray<FTransform> nextTargetTransformArr;
	nextTargetTransformArr.SetNum(m_TargetAmount);
	for (int i = 0; i < m_TargetAmount; ++i)
	{
		nextTargetTransformArr[i] = nextTargetTransform;
		nextTargetTransformArr[i].SetRotation(FQuat(nextTargetTransformArr[i].Rotator().Add(180, rotationPerTarget * i, 0)));
	}
	const FTransformArrayA2 nextTargetTransformArray(targetTransformArr, m_TargetAmount);
	m_DisplayNextTargetInstanced->AddInstances(nextTargetTransformArray, false, false);
	m_DisplayNextTargetInstanced->SetCastShadow(false);
}

float curTime = 0;
// Called every frame
void AVoicePitchDisplay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	curTime += DeltaTime;
	SetDisplayHand(curTime, 100.f);

	if (GetWorld()->GetFirstPlayerController()->GetInputKeyTimeDown(EKeys::Zero) > 1.f)
	{
		HideTarget(1);
		HideNextTarget(12);
	}
	if (GetWorld()->GetFirstPlayerController()->GetInputKeyTimeDown(EKeys::One) > 1.f)
	{
		ShowTarget(1);
		ShowNextTarget(12);
	}
}

void AVoicePitchDisplay::SetDisplayHand(float value, float maxValue, float minValue)
{
	value -= minValue;
	maxValue -= minValue;
	const float scaledValue = (value / maxValue) * 360;

	const FQuat rotation(FRotator(0, scaledValue, 0));
	m_DisplayHand->SetRelativeRotation(rotation);
}

void AVoicePitchDisplay::RotateTargetPitch(UInstancedStaticMeshComponent* target, int targetIndex, double pitch)
{
	targetIndex = targetIndex % target->GetInstanceCount();

	FTransform curInstanceTransform;
	if (target->GetInstanceTransform(targetIndex, curInstanceTransform))
	{
		FRotator rotator = target->GetRelativeRotation();
		rotator.Pitch = pitch;
		rotator.Yaw += (360.f / m_TargetAmount) * targetIndex;
		curInstanceTransform.SetRotation(FQuat4d(FRotator(rotator)));
		target->UpdateInstanceTransform(targetIndex, curInstanceTransform, false, true, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't find instance with index %d."), targetIndex);
	}
}

void AVoicePitchDisplay::StaticMeshToInstance(UStaticMeshComponent* staticMesh, UInstancedStaticMeshComponent* instance)
{
	instance->SetCastShadow(staticMesh->CastShadow);
	instance->SetRelativeTransform(staticMesh->GetRelativeTransform());
	instance->SetStaticMesh(staticMesh->GetStaticMesh());
	int i = 0;
	for (auto* mat : staticMesh->GetMaterials())
	{
		instance->SetMaterial(i++, mat);
	}
	staticMesh->SetVisibility(false);
}

void AVoicePitchDisplay::ShowTarget(int targetIndex)
{
	RotateTargetPitch(m_DisplayTargetInstanced, targetIndex, 0.);
}
void AVoicePitchDisplay::HideTarget(int targetIndex)
{
	RotateTargetPitch(m_DisplayTargetInstanced, targetIndex, 180.);
}
void AVoicePitchDisplay::ShowNextTarget(int targetIndex)
{
	RotateTargetPitch(m_DisplayNextTargetInstanced, targetIndex, 0.);
}

void AVoicePitchDisplay::HideNextTarget(int targetIndex)
{
	RotateTargetPitch(m_DisplayNextTargetInstanced, targetIndex, 180.);
}
