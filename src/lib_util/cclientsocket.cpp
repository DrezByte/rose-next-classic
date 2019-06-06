/*
	$Header: /7HeartsOnline/LIB_Util/CClientSOCKET.cpp 4     04-10-22 11:14a Icarus $
*/

#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include <process.h>
#include "classLOG.h"
#include "CClientSOCKET.h"

#define CLIENTSOCKET_SERVERDEAD		(-1)
#define CLIENTSOCKET_DISCONNECTED	0x000
#define CLIENTSOCKET_CONNECTED		0x001
#define CLIENTSOCKET_CLOSING		0x002


struct t_PACKET {
	union {
		t_PACKETHEADER				m_HEADER;
		BYTE						m_pDATA[ 1 ];
	}  ;
} ;

struct t_SendPACKET {
	WORD			m_wSize;
	union {
		t_PACKET	m_Packet;
		BYTE		m_pDATA[ MAX_PACKET_SIZE ];
	} ;
} ;

//-------------------------------------------------------------------------------------------------
//DWORD WINAPI ClientSOCKET_SendTHREAD (LPVOID lpParameter)
unsigned __stdcall ClientSOCKET_SendTHREAD( void* lpParameter )
{
	CClientSOCKET *pClientSocket = (CClientSOCKET *) lpParameter;

	LOG_DEBUG("ClientSocket_Thread::0x%x", pClientSocket, pClientSocket->m_dwThreadID);

	while ( 1 ) 
	{
		WaitForSingleObject( pClientSocket->m_hThreadEvent, INFINITE ); 

		if ( pClientSocket->m_cStatus != CLIENTSOCKET_CONNECTED ) 
			break;

		::EnterCriticalSection( &pClientSocket->m_csThread );
		pClientSocket->m_SendPacketQ.AppendNodeList( &pClientSocket->m_WaitPacketQ );
		pClientSocket->m_WaitPacketQ.Init ();
        ::LeaveCriticalSection( &pClientSocket->m_csThread );

		if ( pClientSocket->m_bWritable && pClientSocket->m_SendPacketQ.GetNodeCount() > 0 ) {
			if ( !pClientSocket->Packet_Send () ) {
				// ���� ���� �߻�...
				#pragma message ( "!!!!!! >>>> ���� ���� �߻��� ��ó...." )
			}
		} 
#ifdef	_DEBUG
		else
			g_LOG.OutputString( LOG_DEBUG_, " >>>>>>>> Send Failed[ %d Packet(s) ] :: Writable == false \n", pClientSocket->m_SendPacketQ.GetNodeCount() );
#endif
    }

    pClientSocket->m_cStatus = CLIENTSOCKET_DISCONNECTED;

    return (0);
}

bool CClientSOCKET::_Init (void)
{
    m_pRecvPacket  = (t_PACKET *) new char [ MAX_PACKET_SIZE ];
	m_nPacketSize  = 0;
	m_nRecvBytes   = 0;
	m_nSendBytes   = 0;
	m_bWritable	   = false;
//	m_bRecvBlocking= false;
	m_hThread	   = NULL;
	m_hThreadEvent = NULL;

	m_cStatus	  = CLIENTSOCKET_DISCONNECTED;

    ::InitializeCriticalSection( &m_csThread );

	return true;
}


//-------------------------------------------------------------------------------------------------
void CClientSOCKET::_Free (void)
{
    classDLLNODE <t_SendPACKET *> *pSendNode;

    pSendNode = m_WaitPacketQ.GetHeadNode ();
    while ( pSendNode ) {
        m_WaitPacketQ.DeleteNode (pSendNode);

        delete[] pSendNode->DATA;
        delete   pSendNode;

        pSendNode = m_WaitPacketQ.GetHeadNode ();
    }

	// Clear Send Queue.
    pSendNode = m_SendPacketQ.GetHeadNode ();
    while ( pSendNode ) {
        m_SendPacketQ.DeleteNode (pSendNode);

        delete[] pSendNode->DATA;
        delete   pSendNode;

        pSendNode = m_SendPacketQ.GetHeadNode ();
    }

	// Clear Receive Qeueue.
    classDLLNODE <t_PACKET *> *pRecvNode;
    pRecvNode = m_RecvPacketQ.GetHeadNode ();
    while ( pRecvNode ) {
        delete[] pRecvNode->DATA;
        m_RecvPacketQ.DeleteNFree( pRecvNode );

        pRecvNode = m_RecvPacketQ.GetHeadNode ();
    }

	if ( m_pRecvPacket ) {
		delete[] m_pRecvPacket;
		m_pRecvPacket = NULL;
	}

	m_cStatus = CLIENTSOCKET_DISCONNECTED;

    ::DeleteCriticalSection( &m_csThread );
}


//-------------------------------------------------------------------------------------------------
void CClientSOCKET::OnConnect(int nErrorCode)
{
	if ( !nErrorCode ) {
	    classDLLNODE <t_SendPACKET *> *pSendNode;

		// Clear Send Queue.
		pSendNode = m_SendPacketQ.GetHeadNode ();
		while ( pSendNode ) {
			m_SendPacketQ.DeleteNode (pSendNode);

			delete[] pSendNode->DATA;
			delete   pSendNode;

			pSendNode = m_SendPacketQ.GetHeadNode ();
		}

		// Clear Receive Qeueue.
		classDLLNODE <t_PACKET *> *pRecvNode;
		pRecvNode = m_RecvPacketQ.GetHeadNode ();
		while ( pRecvNode ) {
			delete[] pRecvNode->DATA;
			m_RecvPacketQ.DeleteNFree( pRecvNode );

			pRecvNode = m_RecvPacketQ.GetHeadNode ();
		}


		m_cStatus = CLIENTSOCKET_CONNECTED;

		m_hThreadEvent = ::CreateEvent  (NULL, false, false, NULL);
		//m_hThread      = ::CreateThread (NULL, 0, ClientSOCKET_SendTHREAD, this, 0, &m_dwThreadID);
		m_hThread      = (HANDLE)_beginthreadex (NULL, 0, ClientSOCKET_SendTHREAD, this, 0, &m_dwThreadID);
		SetThreadPriority (m_hThread, THREAD_PRIORITY_NORMAL);	// Indicates 1 point  below normal priority for the priority class.
	} else {
		m_cStatus = CLIENTSOCKET_SERVERDEAD;
		this->Close ();
	}
}


//-------------------------------------------------------------------------------------------------
void CClientSOCKET::OnAccepted(int *pCode)
{
	;
}

//-------------------------------------------------------------------------------------------------
void CClientSOCKET::OnClose(int nErrorCode)
{
	Close ();		// Close Socket ...
}


//-------------------------------------------------------------------------------------------------
void CClientSOCKET::OnReceive(int nErrorCode)
{
	DWORD dwBytes;

	if ( !this->IOCtl(FIONREAD, &dwBytes) ) {
		nErrorCode = WSAGetLastError();
		return;
	} 

	Packet_Recv( dwBytes );

	/*
	if ( dwBytes != 0 || nErrorCode != 0 ) {
		if ( !nErrorCode )
			Packet_Recv ();
	} else {
		this->m_bRecvBlocking = true;
	}
	*/
}


//-------------------------------------------------------------------------------------------------
void CClientSOCKET::OnSend (int nErrorCode)
{
	if ( !nErrorCode ) {
		m_bWritable = true;
		::SetEvent( m_hThreadEvent );			// �����忡 �뺸 !!!
	}
}

bool CClientSOCKET::WndPROC(WPARAM wParam, LPARAM lParam)
{
	int nErrorCode = WSAGETSELECTERROR(lParam);
	switch (WSAGETSELECTEVENT(lParam)) {
		case FD_READ:
		{
			this->OnReceive(nErrorCode);
			break;
		}
		case FD_WRITE:
		{
			this->OnSend(nErrorCode);
			break;
		}
		case FD_CONNECT:
		{
			this->OnConnect(nErrorCode);

			if (!nErrorCode)
				this->Set_NetSTATUS(NETWORK_STATUS_CONNECT);
			else
				this->Set_NetSTATUS(NETWORK_STATUS_DERVERDEAD);
			break;
		}
		case FD_CLOSE:
		{
			this->OnClose(nErrorCode);
			this->Set_NetSTATUS(NETWORK_STATUS_DISCONNECT);
		}
	}

	return true;
}

void CClientSOCKET::Packet_Register2RecvQ( t_PACKET *pRegPacket )
{
	t_PACKET *pNewPacket;
	pNewPacket  = (t_PACKET *) new char[ pRegPacket->m_HEADER.m_nSize ];
	if ( pNewPacket ) {
		::CopyMemory (pNewPacket, pRegPacket, pRegPacket->m_HEADER.m_nSize);
		m_RecvPacketQ.AllocNAppend( pNewPacket );
	}
}

void CClientSOCKET::Set_NetSTATUS (BYTE btStatus)
{	
	m_pRecvPacket->m_HEADER.m_wType		= SOCKET_NETWORK_STATUS;
	m_pRecvPacket->m_HEADER.m_nSize		= sizeof( t_NETWORK_STATUS );
	( (t_NETWORK_STATUS*)m_pRecvPacket)->m_btStatus	= btStatus;

	this->Packet_Register2RecvQ( m_pRecvPacket );
}

//-------------------------------------------------------------------------------------------------
void CClientSOCKET::Packet_Register2SendQ (t_PACKET *pRegPacket)
{
	if (m_cStatus != CLIENTSOCKET_CONNECTED) {
		return;
	}

	t_SendPACKET *pSendPacket = new t_SendPACKET();
	if (!pSendPacket) {
		return;
	}

//	if ( m_cStatus == CLIENTSOCKET_CONNECTED ) 
	{
		::EnterCriticalSection( &m_csThread );
		{
			::CopyMemory(&pSendPacket->m_Packet, pRegPacket, pRegPacket->m_HEADER.m_nSize);
			pSendPacket->m_wSize = pSendPacket->m_Packet.m_HEADER.m_nSize;
			m_WaitPacketQ.AllocNAppend( pSendPacket );
		}
		::LeaveCriticalSection( &m_csThread );

		::SetEvent( m_hThreadEvent );
	}
}


//-------------------------------------------------------------------------------------------------
void CClientSOCKET::Packet_Recv( int iToRecvBytes )
{
    int iRecvBytes;

	do {
		if ( this->m_nRecvBytes < sizeof(t_PACKETHEADER) ) {
			iRecvBytes = this->Receive ((char*)m_pRecvPacket + this->m_nRecvBytes, sizeof(t_PACKETHEADER)-this->m_nRecvBytes, 0);
		} else {
			//iRecvBytes = this->Receive ((char*)m_pRecvPacket + m_nRecvBytes, m_pRecvPacket->m_HEADER.m_nSize - m_nRecvBytes, 0);
			iRecvBytes = this->Receive ((char*)m_pRecvPacket + this->m_nRecvBytes, this->m_nPacketSize - this->m_nRecvBytes, 0);
		}

		if ( iRecvBytes == SOCKET_ERROR ) {
			int WSAErr = WSAGetLastError ();
			if ( WSAErr != WSAEWOULDBLOCK ) {
				// LOST  Connection !!!
				// CLOSE Connection ...

				Socket_Error ("Packet_Recv ...");
				break;
			}
			break;
		} else
		if ( iRecvBytes <= 0 )
			return;

		this->m_nRecvBytes += iRecvBytes;

		if ( this->m_nRecvBytes >= sizeof(t_PACKETHEADER) ) {
			if ( this->m_nRecvBytes == sizeof(t_PACKETHEADER) ) {
				this->m_nPacketSize = m_pRecvPacket->m_HEADER.m_nSize;
				if ( !this->m_nPacketSize ) {
					this->Close ();
					return;
				}
			}

			_ASSERT( this->m_nPacketSize );
			_ASSERT( this->m_nRecvBytes <= this->m_nPacketSize );

			if ( this->m_nRecvBytes >= this->m_nPacketSize ) {
				t_PACKET *pNewPacket = (t_PACKET *) new char[ this->m_nPacketSize ];

				if ( pNewPacket ) {
					::CopyMemory (pNewPacket, m_pRecvPacket, this->m_nPacketSize);

					m_RecvPacketQ.AllocNAppend( pNewPacket );
				}

				this->m_nPacketSize = 0;
				this->m_nRecvBytes = 0;
			}
		}

		iToRecvBytes -= iRecvBytes;
	} while( iToRecvBytes > 0 );
}


//-------------------------------------------------------------------------------------------------
bool CClientSOCKET::Packet_Send (void)
{
    classDLLNODE <t_SendPACKET *> *pNode;
    int iRetValue;

    while ( m_SendPacketQ.GetNodeCount() > 0 ) {
        pNode = m_SendPacketQ.GetHeadNode();

		iRetValue = this->Send ((char*)pNode->DATA->m_pDATA + m_nSendBytes, pNode->DATA->m_wSize - m_nSendBytes, 0);
		if ( iRetValue == SOCKET_ERROR ) {
			int WSAErr = WSAGetLastError ();

			if ( WSAErr != WSAEWOULDBLOCK ) {
				// LOST  Connection !!!
				// CLOSE Connection ...
				Socket_Error ("Packet_Send ...");
				return false;
			}

			m_bWritable = false;
			return true;
		}

		m_nSendBytes += iRetValue;
		if ( m_nSendBytes == pNode->DATA->m_wSize ) {
			classDLLNODE <t_SendPACKET *> *pDelNode;

			pDelNode = pNode;
			pNode = m_SendPacketQ.GetNextNode (pDelNode);

			delete pDelNode->DATA;
			m_SendPacketQ.DeleteNFree( pDelNode );

			m_nSendBytes = 0;
        }
    }

	return true;
}

bool CClientSOCKET::Peek_Packet (t_PACKET *pPacket, bool bRemoveFromQ)
{
	if ( this->m_RecvPacketQ.GetNodeCount() > 0 ) {
        classDLLNODE <t_PACKET*> *pNode;

		pNode   = this->m_RecvPacketQ.GetHeadNode();
		// pPacket = pNode->DATA;
		CopyMemory (pPacket, pNode->DATA, pNode->DATA->m_HEADER.m_nSize);

        // ��Ŷ ����.
		if ( bRemoveFromQ ) {
			this->m_RecvPacketQ.DeleteNode ( pNode );
			delete[] pNode->DATA;
			delete   pNode;
		}

		return true;
	}

	return false;
}

bool CClientSOCKET::Connect (HWND hWND, char *szServerIP, int iTCPPort, UINT uiWindowMsg)
{
	bool bReturn;

	if ( INVALID_SOCKET == this->m_hSocket ) {
		bReturn = this->Create(hWND, uiWindowMsg);
		if ( !bReturn ) {
			OutputDebugString ("Connect :: Create() Failure !!!\n");
			return false;
		}
	}

	bReturn = CRawSOCKET::Connect (szServerIP, iTCPPort);
	if ( !bReturn ) {
		OutputDebugString ("Connect :: Connect() Failure !!!\n");
		return false;
	}

//	_AppendSocketList (pClientSocket);

	return true;
}

//-------------------------------------------------------------------------------------------------
void CClientSOCKET::Close ()
{
	if ( m_hThread != NULL ) {
	    m_cStatus = CLIENTSOCKET_CLOSING;

		do {
			::SetEvent( m_hThreadEvent );				// ������ ������� �뺸...
			Sleep (100);
		} while ( m_cStatus == CLIENTSOCKET_CLOSING ) ;

		::CloseHandle (m_hThread);
		m_dwThreadID = 0;
		m_hThread    = NULL;
	}

	if ( m_hThreadEvent ) {
		::CloseHandle( m_hThreadEvent );
		m_hThreadEvent = NULL;
	}

	CRawSOCKET::Close();
}


//-------------------------------------------------------------------------------------------------
