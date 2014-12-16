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

#ifndef __C_NETWORKMGR_H_INCLUDED__
#define __C_NETWORKMGR_H_INCLUDED__

/****************************************************************/
/*                        Defines                               */
/****************************************************************/

/****************************************************************/
/*                        Include                               */
/****************************************************************/

#include "enet/enet.h"
#include "message.hpp"
#include "AllMsg.h"


#include <string>
#include <vector>

/****************************************************************/
/*                        Namespace                             */
/****************************************************************/

/****************************************************************/
/*                       Class									*/
/****************************************************************/

class NetworkManagerSr
{
public:
    // States for the finite state machine. First for server:
    enum NetworkState {NS_MAIN_MENU,						// Estado Inicial
                       NS_ACCEPT_CONNECTIONS,				// server: esperando conexiones
                       NS_MODEL_TO_USE,						// Enviando Modelos usados
					   NS_MODEL_LOADING,					// Cargando los modelos de la Pista
					   NS_UPDATING							// Enviando Actializaciones de la Pista
    };
private:

    NetworkState                m_state;
	int							Max_clients;
    ENetHost*                   m_host;						// Server
	unsigned int                m_num_clients;				// Actual numero de clientes
    std::vector<ENetPeer*>      m_clients;					// Vector con los "Sockets" de los clientes
	std::vector<std::string>    m_client_names;				// Nombres de los clientes

public:
	NetworkManagerSr();
	~NetworkManagerSr();

	//Propiedades
	int getMaxClients()								{return Max_clients;	}
	void setState(NetworkState s)					{m_state = s;			}
    NetworkState getState() const					{return m_state;		}
	unsigned int getNumClients() const				{return m_num_clients;	}

    void update(float dt);
	bool initServer(int port);
	void handleNewConnection(ENetEvent *inevent);
	void handleDisconnection(ENetEvent *inevent);
	void broadcastToClients(Message *m);
};


/****************************************************************/
/*                        Global                                */
/****************************************************************/

extern NetworkManagerSr* MgrNetwork;

/****************************************************************/
/*                       Functions								*/
/****************************************************************/


#endif // __C_NETWORKMGR_H_INCLUDED__