/************************************************************************************ 
 * Copyright (c) 2014-2015, TU Ilmenau
 * 
 * Build with Irrlicht framework
 * STVR use:
 *                  Vuzix SDK 3.3
 *					Irrlicht 1.8
 *                  IrrEdit 4.5
 *					DirectX 9
 * Viel Dank guys
 * ===================================================================================
 * Authors:  Luis Rojas (luis-alejandro.rojas-vargas@tu-ilmenau.de) 
 *************************************************************************************/


/****************************************************************/
/*                        Include                               */
/****************************************************************/

#include "NetworkMgr.h"

/****************************************************************/
/*                        Global                                */
/****************************************************************/

NetworkManagerSr* MgrNetwork = 0;

/****************************************************************/
/*                       Functions								*/
/****************************************************************/

NetworkManagerSr::NetworkManagerSr()
{
     m_state          = NS_MAIN_MENU;
     m_host           = NULL;

     m_num_clients    = 0;

	 Max_clients = 10;

     if (enet_initialize () != 0)
     {
      fprintf (stderr, "An error occurred while initializing ENet.\n");
      exit(-1);
     }
}   // NetworkManager

NetworkManagerSr::~NetworkManagerSr()
{
     enet_host_destroy(m_host);
     enet_deinitialize();
}   // ~NetworkManager

void NetworkManagerSr::update(float dt)
{
    // No deberia estar en este estado nunca
    if(m_state==NS_MAIN_MENU) return;

    ENetEvent event;
    int result = enet_host_service (m_host, &event, 0);

    if(result==0)	//No hay evento
		return;

    if(result<0)	//Error en el Mensaje
    {
        fprintf(stderr, "Error while receiving messages -> ignored.\n");
        return;
    }

    switch (event.type)
    {
		case ENET_EVENT_TYPE_CONNECT:
			handleNewConnection(&event);
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			//handleMessageAtServer(&event);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			handleDisconnection(&event);
			break;
		case ENET_EVENT_TYPE_NONE:
			break;
    }
}   // update

bool NetworkManagerSr::initServer(int port)
{
     ENetAddress address;
     address.host = ENET_HOST_ANY;
     address.port = port;
	 
     m_host = enet_host_create (& address     /* the address to bind the server host to */,
			NetworkManagerSr::getMaxClients() /* number of connections */,
                                0             /* channel limit */,
                                0             /* incoming bandwidth */,
                                0             /* outgoing bandwidth */     );
    if (m_host == NULL)
    {
        fprintf (stderr,
                 "An error occurred while trying to create an ENet server host.\n"
                 "Progressing in non-network mode\n");
        return false;
    }

    m_clients.push_back(NULL); // server has host_id=0, so put a dummy entry at 0 in client array

    m_client_names.push_back("server");
    return true;
}   // initServer

void NetworkManagerSr::handleNewConnection(ENetEvent *inevent)
{
    printf ("A new client connected from %x:%u.\n", 
                inevent->peer -> address.host,
                inevent->peer -> address.port);

	// Only accept while waiting for connections
    if(m_state!=NS_ACCEPT_CONNECTIONS)
		return;

    // The logical connection (from STK point of view) happens when
    // the connection message is received. But for now reserve the
    // space in the data structures (e.g. in case that two connects
    // happen before a connect message is received
    m_client_names.push_back("NOT SET YET");
    m_clients.push_back(inevent->peer);
    inevent->peer->data = (void*)int(m_clients.size()-1);  // save hostid in peer data

	m_num_clients++;

}   // handleNewConnection

void NetworkManagerSr::handleDisconnection(ENetEvent *inevent)
{
    if(m_state!=NS_ACCEPT_CONNECTIONS)
    {
        fprintf(stderr, "Disconnect while in race - close your eyes and hope for the best.\n");
        return;
    }
    fprintf(stderr, "%x:%d disconnected (host id %d).\n", inevent->peer->address.host,
        inevent->peer->address.port, (int)(long)inevent->peer->data );
    m_num_clients--;
}   // handleDisconnection

void NetworkManagerSr::broadcastToClients(Message *m)
{
    enet_host_broadcast(m_host, 0, m->getPacket());
	//enet_peer_send(m_clients[1], 0, m.getPacket());
    enet_host_flush(m_host);
}   // broadcastToClients

