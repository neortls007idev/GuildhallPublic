#pragma once
#include "Engine/Memory/Job.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class MostAwesomeGameplayJob : public Job
{
public:
	MostAwesomeGameplayJob( int MostAwesomeJobID );
	~MostAwesomeGameplayJob();
	
	virtual void Execute() override;

public:
		
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------