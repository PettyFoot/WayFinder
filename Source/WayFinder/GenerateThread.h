// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Public/HAL/Runnable.h"

//Forward Declarations
class FRunnableThread;
class AChunkGenerator;


class AGenerateThread : public FRunnable
{
	
public:	
	// Sets default values for this actor's properties
	AGenerateThread();
	AGenerateThread(int num_calcs, AChunkGenerator* chunk_generator);

	bool bStopThread;

	virtual bool Init();

	virtual uint32 Run();

	virtual void Stop();

private:

	int32 Calculations;

	int32 CalcCount;

	AChunkGenerator* CurrentChunkGenerator;

	int32 CurrentCalculation;


};
