//-----------------------------------------------------------------------------------------------
// UnitTests_Custom.cpp
//
// YOU MAY CHANGE anything in this file.  Make sure your "RunTestSet()" commands at the bottom
//	of the file in RunTest_Custom() all pass "false" for their first argument (isGraded).
//
#include "Game/UnitTests_Custom.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
// Include whatever you want here to test various engine classes (even non-math stuff, like Rgba)

//-----------------------------------------------------------------------------------------------
int TestSet_Custom_Dummy()
{
	VerifyTestResult( 2 == 2, "Dummy example test that passes (2 == 2)" ); // e.g. a non-graded test that passes
	VerifyTestResult( 2 == 3, "Dummy example test that fails (2 == 3)" ); // e.g. a non-graded test that doesn't pass

	return 2; // Number of tests expected (equal to the # of times you call VerifyTestResult)
}


//-----------------------------------------------------------------------------------------------
int TestSet_Custom_Rgba()
{
	// Write your own tests here; each call to VerifyTestResult is considered a "test".

	return 0; // Number of tests expected (set equal to the # of times you call VerifyTestResult)
}


//-----------------------------------------------------------------------------------------------
void RunTests_Custom()
{
	// Always set first argument to "false" for Custom tests, so they don't interfere with grading
	RunTestSet( false, TestSet_Custom_Dummy,	"Custom dummy sample tests" );
	RunTestSet( false, TestSet_Custom_Rgba,		"Custom Rgba sample tests" );
}

