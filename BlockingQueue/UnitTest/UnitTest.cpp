#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS( UnitTest )
	{
	public:

		TEST_METHOD_INITIALIZE( InitUDPSocketTest )
		{
			
		}
		
		TEST_METHOD( UDPSocketTest )
		{
			Logger::WriteMessage( "Starting UDP SOcket Test" );

			Assert::AreEqual( true , true , L"They match" );
			Assert::AreEqual( 5 , 6 , L"They Don't match" );
		}

		TEST_METHOD_CLEANUP( DeInitUDPSocketTest )
		{
			
		}
	};
}
