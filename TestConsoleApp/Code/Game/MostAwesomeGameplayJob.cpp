#include "Game/MostAwesomeGameplayJob.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include <cstdio>


//--------------------------------------------------------------------------------------------------------------------------------------------


MostAwesomeGameplayJob::MostAwesomeGameplayJob( int MostAwesomeJobID ) :
																		 Job()
{
	UNUSED( MostAwesomeJobID );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

MostAwesomeGameplayJob::~MostAwesomeGameplayJob()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void MostAwesomeGameplayJob::Execute()
{
	int x = 5;
	for( int index = 0 ; index < 1000 ; index++ )
	{
		x++;
		x ^= 109876;
		x = x << 4;
		x &= 12768931;

		for ( int index2 = 1 ; index2 < 1000 ; index2++   )
		{
			x *= index;
			x /= index2;
		}
	}
	printf( "\nJob Completed = %d" , m_jobID );

	ClaimJobCallbackOnComplete();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
