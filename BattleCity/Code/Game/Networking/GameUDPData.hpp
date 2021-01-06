#pragma once

enum UDPDataPacketType
{
	DATA_ACK ,
	DATA_NACK ,
	TANK_ENTITY_DATA ,
	BULLET_ENTITY_DATA ,
	PLAYER_ENTITY_DATA ,
	EXPLOSION_ENTITY_DATA ,

	NUM_PACKET_DATA ,
};

enum PacketDataBreakDown
{
	D_UNIQUE_ID,
	D_FRAME_ID,
	D_ENTITY_TYPE,
	D_ENTITY_ID,
	D_POSITION,
	D_ORIENTATION_DEG,
	D_FACTION,
	D_HEALTH,
	D_GARBAGE,

	D_NUM_TOTAL
};

struct GameUDPData
{
public:
	int					m_frameID;
	UDPDataPacketType	m_packetData;
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------