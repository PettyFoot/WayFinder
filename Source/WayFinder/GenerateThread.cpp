// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerateThread.h"
#include "ChunkGenerator.h"

// Sets default values
AGenerateThread::AGenerateThread()
{


}

AGenerateThread::AGenerateThread(int num_calcs, AChunkGenerator* chunk_generator)
{
	if (num_calcs > 0 && chunk_generator)
	{
		this->Calculations = num_calcs;
		this->CurrentChunkGenerator = chunk_generator;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AGenerateThread::Constructor__ calculations must be greater than 0 or no chunk generator"));
	}
	
}

bool AGenerateThread::Init()
{
	bStopThread = false;
	CalcCount = 0;


	return true;
}

uint32 AGenerateThread::Run()
{
	while (!bStopThread)
	{
		if (CalcCount < Calculations)
		{
			this->CurrentChunkGenerator->GenerateTerrain();
			//UE_LOG(LogTemp, Warning, TEXT("calc count %d"), CalcCount);
			CalcCount++;
		}
		else
		{
			bStopThread = true;
			this->CurrentChunkGenerator->EndGeneration();
		}
	}
	return 0;
}

void AGenerateThread::Stop()
{
}

