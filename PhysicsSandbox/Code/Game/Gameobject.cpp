#include "Game/Gameobject.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

GameObject::GameObject( Physics2D* system , Vec2 rigidBodyPosition , Vec2 coliderPositionRelativeToRigidBody , float ColliderRadius ) :
																													m_physiscsSystem( system )
{
	m_rigidbody = m_physiscsSystem->CreateRigidbody( rigidBodyPosition , coliderPositionRelativeToRigidBody , ColliderRadius );
}


GameObject::GameObject( Physics2D* system , Vec2 rigidBodyPosition , Vec2 coliderPositionRelativeToRigidBody , Polygon2D convexgon ) :
																														m_physiscsSystem( system )
{
	m_rigidbody = m_physiscsSystem->CreateRigidbody( rigidBodyPosition , coliderPositionRelativeToRigidBody , convexgon );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Strings GameObject::GetGameObjectInfo() const
{
	Strings objectDetails;
	std::string trueTextForBoolean = "true";
	std::string FalseTextForBoolean = "false";

	Vec2 velocity						= m_rigidbody->GetVelocity();
	Vec2 verletVelocity					= m_rigidbody->GetVerletVelocity();
	Vec2 position						= m_rigidbody->GetPosition();
	float mass							= m_rigidbody->GetMass();
	float friction						= m_rigidbody->GetCollider()->GetPhysicsMaterial()->GetFriction();
	float drag							= m_rigidbody->GetDrag();

	float Bounce						= m_rigidbody->GetCollider()->GetPhysicsMaterial()->GetBounciness();
	float angularVelocityDegrees		= m_rigidbody->GetAngularVelocityInDegrees();
	float rotationDegrees				= m_rigidbody->GetRotationInDegrees();
	float moment						= m_rigidbody->GetMoment();
	float angularDrag					= m_rigidbody->GetAngularDrag();

	
	std::string objectPos = "           Position = ";
	objectPos += std::to_string( position.x ) + " , " + std::to_string( position.y );

	std::string objectVelocity = "           Velocity = ";
	objectVelocity += std::to_string( velocity.x ) + " , " + std::to_string( velocity.y );

	std::string objectVerletVelocity = "           Verlet Velocity = ";
	objectVerletVelocity += std::to_string( verletVelocity.x ) + " , " + std::to_string( verletVelocity.y );
	
	std::string objectMass = " [  ,  ] : Mass = ";
	objectMass += std::to_string( mass );

	std::string objectSimMode = " 1 , 2 , 3 : Simulation Mode = ";

	std::string objectFriction = " <- , -> : Friction = ";
	objectFriction += std::to_string( friction );

	std::string objectDrag = " :  ,  \\ : Drag = ";
	objectDrag += std::to_string( drag );

	std::string objectBounce = "         : Bounciness = ";
	objectBounce += std::to_string( Bounce );

	std::string objectRotation = " R  ,  F : Rotation In Degrees = ";
	objectRotation += std::to_string( rotationDegrees );

	std::string objectAngularVelocity = " T , G , V : Angular Velocity In Degrees = ";
	objectAngularVelocity += std::to_string( angularVelocityDegrees );

	std::string objectAngularDrag = " K  ,  L : Angular Drag = ";
	objectAngularDrag += std::to_string( angularDrag );

	std::string objectMoment = "         : Moment = ";
	objectMoment += std::to_string( moment );
	
	switch ( m_rigidbody->GetSimulationMode() )
	{
		case SIMULATIONMODE_DYNAMIC :
									objectSimMode += "DYNAMIC";
									break;
		case SIMULATIONMODE_STATIC:
									objectSimMode += "STATIC";
									break;
		case SIMULATIONMODE_KINEMATIC:
									objectSimMode += "KINEMATIC";
									break;
	}

	
	objectDetails.push_back( objectSimMode );
	objectDetails.push_back( objectMass );
	objectDetails.push_back( objectDrag );
	objectDetails.push_back( objectFriction );
	objectDetails.push_back( objectVelocity );
	objectDetails.push_back( objectVerletVelocity );
	objectDetails.push_back( objectPos );

	objectDetails.push_back( objectBounce );
	objectDetails.push_back( objectAngularDrag );
	objectDetails.push_back( objectRotation );
	objectDetails.push_back( objectAngularVelocity );
	objectDetails.push_back( objectMoment );
		
	return objectDetails;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameObject::~GameObject()
{
	// delete m_rigidbody; // this should be an error if setup right
	m_rigidbody->Destroy(); // destroys through the system;  

}

//--------------------------------------------------------------------------------------------------------------------------------------------