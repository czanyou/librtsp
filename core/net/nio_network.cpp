/***
 * The content of this file or document is CONFIDENTIAL and PROPRIETARY
 * to ChengZhen(Anyou).  It is subject to the terms of a
 * License Agreement between Licensee and ChengZhen(Anyou).
 * restricting among other things, the use, reproduction, distribution
 * and transfer.  Each of the embodiments, including this information and
 * any derivative work shall retain this copyright notice.
 *
 * Copyright (c) 2014-2015 ChengZhen(Anyou). All Rights Reserved.
 *
 */
#include "stdafx.h"

#include "nio_socket.h"
#include "nio_manager.h"


namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SocketAddress class

SocketAddress::SocketAddress()
{
	memset(fAddress, 0, sizeof(fAddress));
	fPort = 0;
}

SocketAddress::SocketAddress( const SocketAddress& address )
{
	operator = (address);
}

SocketAddress::SocketAddress( LPCSTR address )
{
	memset(fAddress, 0, sizeof(fAddress));
	fPort = 0;

	SetAddress(address);
}

void SocketAddress::Clear()
{
	memset(fAddress, 0, sizeof(fAddress));
	fPort = 0;
}

String SocketAddress::GetAddress()
{
	char buffer[MAX_PATH];
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%d.%d.%d.%d", 
		fAddress[0], fAddress[1], fAddress[2], fAddress[3]);
	return buffer;
}

BYTE* SocketAddress::GetAddressBytes()
{
	return fAddress;
}


UINT SocketAddress::GetPort()
{
	return fPort;
}

BOOL SocketAddress::IsIpAddress( LPCSTR hostName )
{
	struct sockaddr_in addr_in;
	memset(&addr_in, 0, sizeof(struct sockaddr_in));	
	if (inet_aton(hostName, &addr_in.sin_addr) == 0) {
		return FALSE;
	}

	return TRUE;
}

/** Returns TRUE if nIP is a LAN ip, FALSE otherwise. */
BOOL SocketAddress::IsLocalAddress()
{
	// filter LAN IP's
	// -------------------------------------------
	// 0.*
	// 10.0.0.0 - 10.255.255.255  class A
	// 172.16.0.0 - 172.31.255.255  class B
	// 192.168.0.0 - 192.168.255.255 class C

	if (fAddress[0] == 192) { // check this 1st, because those LANs IPs are mostly spreaded
		return (fAddress[1] == 168);

	} else if (fAddress[0] == 172) {
		return (fAddress[1] >= 16) && (fAddress[1] <= 31);

	} else if (fAddress[0] == 10) {
		return TRUE;

	} else if (fAddress[0] == 0) {
		return TRUE;
	}

	return FALSE; 
}

BOOL SocketAddress::IsValidAddress()
{
	UINT value = fAddress[0] << 24;
	value += fAddress[1] << 16;
	value += fAddress[2] << 8;
	value += fAddress[3];

	if (value == 0) {
		return FALSE;

	} else if (fPort == 0) {
		return FALSE;
	}

	return TRUE;
}

void SocketAddress::SetAddress( LPCSTR address )
{
	if (address == NULL) {
		memset(fAddress, 0, sizeof(fAddress));
		return;
	}

	if (address == NULL) {
		return;
	}

	UINT a[4];
	sscanf(address, "%u.%u.%u.%u", &a[0], &a[1], &a[2], &a[3]);

	fAddress[0] = a[0];
	fAddress[1] = a[1];
	fAddress[2] = a[2];
	fAddress[3] = a[3];
}

void SocketAddress::SetAddress( const BYTE* address )
{
	if (address == NULL) {
		return;
	}

	memcpy(fAddress, address, 4);
}

void SocketAddress::SetPort( UINT port )
{
	fPort = port;
}

String SocketAddress::ToString()
{
	char buffer[MAX_PATH];
	memset(buffer, 0, sizeof(buffer));

	if (fPort == 0) {
		sprintf(buffer, "%d.%d.%d.%d", 
			fAddress[0], fAddress[1], fAddress[2], fAddress[3]);

	} else {
		sprintf(buffer, "%d.%d.%d.%d:%u", 
			fAddress[0], fAddress[1], fAddress[2], fAddress[3], fPort);
	}

	return buffer;
}

SocketAddress& SocketAddress::operator=( const SocketAddress& address )
{
	if (this == &address) {
		return *this;
	}

	SetAddress(address.fAddress);
	SetPort(address.fPort);

	return *this;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SocketListener class

SocketListener::SocketListener()
{

}

void SocketListener::OnRead( int type, ByteBuffer* buffer, SocketConnection* connection )
{

}

void SocketListener::OnSend( int type, ByteBuffer* buffer, SocketConnection* connection )
{

}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SocketConnection class

SocketConnection::SocketConnection()
{
	fTimeout = 10000;
	fBufferSize = 1024 * 64;
}

int SocketConnection::Connect( UINT socketPort, LPCSTR socketAddress /*= NULL*/ )
{
	return 0;
}

int SocketConnection::Close()
{
	return 0;
}

int SocketConnection::GetBufferSize()
{
	return 0;
}

int SocketConnection::Write( BYTE* data, UINT size )
{
	return 0;
}

int SocketConnection::End()
{
	return 0;
}

int SocketConnection::SetTimeout( int timeout )
{
	return 0;
}

SocketAddress SocketConnection::GetLocalAddress()
{
	return fLocalAddress;
}

SocketAddress SocketConnection::GetRemoteAddress()
{
	return fRemoteAddress;
}

int SocketConnection::SetListener( SocketListener* listener )
{
	fListener = listener;
	return 0;
}

int SocketConnection::IsInvalid()
{
	return 0;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SocketServer class

SocketServer::SocketServer()
{

}

int SocketServer::Listen( UINT socketPort, LPCSTR socketAddress /*= NULL*/, int backlog /*= 5*/ )
{
	return 0;
}

int SocketServer::Close()
{
	return 0;
}

int SocketServer::GetConnectionCount()
{
	return 0;
}

int SocketServer::SetListener( SocketListener* listener )
{
	fListener = listener;
	return 0;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SocketManager class

SocketManager::SocketManager()
{
}

SocketServerPtr SocketManager::CreateServer()
{
	return NULL;
}

void SocketManager::Close()
{

}

int SocketManager::Open()
{
	return 0;
}

void SocketManager::WaitEvents( int timeout )
{

}




} //
}
