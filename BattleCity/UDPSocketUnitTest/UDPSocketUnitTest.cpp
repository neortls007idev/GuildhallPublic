#include "pch.h"
#include "CppUnitTest.h"

#include "Engine/Networking/UDPSocket.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Platform/Window.hpp"

#include <WinSock2.h>
#include "ws2tcpip.h"
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <array>
#include <mutex>
#include <tuple>
#include <string>

#define UNIT_TEST

#pragma warning( push )
#pragma warning ( disable: 4244 )

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern InputSystem*		g_theInput;
extern DevConsole*		g_theDevConsole;
Window*					g_theWindow		= nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NetworkingUnitTest
{
	TEST_CLASS(UDPSocketUnitTest)
	{
	public:
		
		TEST_METHOD_INITIALIZE( InitUDPSocketTest )
		{
			WSAData wsaData;
			WORD wVersion MAKEWORD( 2 , 2 );
			int iResult = WSAStartup( wVersion , &wsaData );

			if ( iResult != 0 )
			{
				std::cout << "Call to WSAStartup Failed " << WSAGetLastError() << std::endl;
			}
		}

		using TextArray = std::array<std::string , 5 >;
		TextArray text = {
			"Hello",
			"Watch out UDP message incoming",
			"This is a UDP message",
			"Thanks for recieving my message",
			"Goodbye"
		};

		std::uint16_t toU16( size_t input )
		{
			return static_cast< std::uint16_t >( input );
		}

		using MessageArray = std::array< std::tuple< std::uint16_t , std::uint16_t , std::uint16_t , std::string > , 5 >;
		MessageArray messages = {
			std::make_tuple( 1, 0, toU16( strlen( text[ 0 ].c_str() ) ) , text[ 0 ] ),
			std::make_tuple( 1, 1, toU16( strlen( text[ 1 ].c_str() ) ) , text[ 1 ] ),
			std::make_tuple( 1, 2, toU16( strlen( text[ 2 ].c_str() ) ) , text[ 2 ] ),
			std::make_tuple( 1, 3, toU16( strlen( text[ 3 ].c_str() ) ) , text[ 3 ] ),
			std::make_tuple( 0, 4, toU16( strlen( text[ 4 ].c_str() ) ) , text[ 4 ] ),
		};

		static void writer( UDPSocket& socket, MessageArray const& messages )
		{
			UDPMessageHeader header;
			for( auto msg : messages )
			{
				header.id			= std::get<0>( msg );
				header.length		= std::get<2>( msg );
				header.sequenceNo	= std::get<1>( msg );

			auto& buffer = socket.SendBuffer();
			*reinterpret_cast< UDPMessageHeader* >( &buffer[ 0 ] ) = header;

			memcpy( &( socket.SendBuffer()[ sizeof( UDPMessageHeader ) ] ) , std::get<3>( msg ).c_str() , header.length );

			socket.SendBuffer()[ sizeof( UDPMessageHeader ) + header.length ] = NULL;
			socket.Send( sizeof( UDPMessageHeader ) + header.length + 1 );

			}
		}

		static void reader( UDPSocket& socket , TextArray& messages )
		{
			static std::mutex lock;

			UDPMessageHeader const* pMsg = nullptr;
			std::string dataStr;
			size_t length = 0;

			do 
			{
				length = socket.Receive();

				dataStr.clear();

				if( length > 0 )
				{
					auto& buffer = socket.ReceiveBuffer();

					pMsg = reinterpret_cast< UDPMessageHeader const* >( &buffer[ 0 ] );
					if( pMsg->length > 0 )
					{
						dataStr = &buffer[ sizeof( UDPMessageHeader ) ];
					}
					messages[ pMsg->sequenceNo ] = dataStr;
					{
						std::lock_guard<std::mutex> guard( lock );

						std::ostringstream ostr;
						ostr << "Thread [ " << std::this_thread::get_id() << " ] recieved Message"
							 << " id = " << pMsg->id
							 << " size = " << pMsg->length
							 << " sequence num = " << pMsg->sequenceNo
							 << " data = " << dataStr
							 << std::endl << std::ends;
						
						Logger::WriteMessage( ostr.str().c_str() );
					}
				}
			} while ( pMsg != nullptr && pMsg->id != 0 );
		}
		
		BEGIN_TEST_METHOD_ATTRIBUTE( UDPSocketTest )
			TEST_DESCRIPTION(L"Testing Concurrent read and write on UDP Sockets")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD( UDPSocketTest )
		{
			Logger::WriteMessage( "Starting UDP Socket Test" );

			UDPSocket test1UDPSocket( "127.0.0.1" , 48000 );
			test1UDPSocket.Bind( 48001 );

			UDPSocket test2UDPSocket( "127.0.0.1" , 48001 );
			test2UDPSocket.Bind( 48000 );

			TextArray readMessageOne;
			TextArray readMessageTwo;

			std::thread writerOne( &UDPSocketUnitTest::writer , std::ref( test1UDPSocket ) , std::cref( messages ) );
			
			std::thread readerOne( &UDPSocketUnitTest::reader , std::ref( test1UDPSocket ) , std::ref( readMessageOne ) );
			std::thread readerTwo( &UDPSocketUnitTest::reader , std::ref( test2UDPSocket ) , std::ref( readMessageTwo ) );

			std::thread writerTwo( &UDPSocketUnitTest::writer , std::ref( test2UDPSocket ) , std::cref( messages ) );
			
			writerOne.join();
			writerTwo.join();
			readerOne.join();
			readerTwo.join();

			Assert::IsTrue( text == readMessageOne );
			Assert::IsTrue( text == readMessageTwo );
		}

		TEST_METHOD_CLEANUP( DeInitUDPSocketTest )
		{
			int iResult = WSACleanup();

			if ( iResult == SOCKET_ERROR )
			{
				std::cout << "Call to WSACleanup Failed " << WSAGetLastError() << std::endl;
			}
		}
	};
}
