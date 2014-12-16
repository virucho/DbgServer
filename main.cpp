

#include <irrlicht.h>

//#include <enet/enet.h>
#include "NetworkMgr.h"
#include "ObjectsMgr.h"

using namespace irr;			//Haupt Namespace von Irrlicht

using namespace core;			//Zusätzliche Namespace

vector3df Posicion = vector3df(0.0f, 0.0f, 0.0f);
vector3df Rotacion = vector3df(0.0f, 0.0f, 0.0f);
vector3df Escalado = vector3df(1.0f, 1.0f, 1.0f);

#define SVR_PASO	0.01f
#define SVR_ESCALO	0.05f
#define SVR_ROTA	1.00f

void UpdateMov()
{
	if(Posicion.X < 40.0f)
		Posicion.X += SVR_PASO;
	else
		Posicion.X = 0.0f;

	if(Rotacion.X < 359.0f)
		Rotacion.X += SVR_ROTA;
	else
		Rotacion.X = 0.0f;

	if(Escalado.X < 2.0f)
	{
		Escalado.X += SVR_PASO;
		Escalado.Y += SVR_PASO;
		Escalado.Z += SVR_PASO;
	}
	else
	{
		Escalado.X = 1.0f;
		Escalado.Y = 1.0f;
		Escalado.Z = 1.0f;
	}
}


void main()
{
	int Port = 2305;

	ObjeScene* Pista = new ObjeScene();
	Pista->setModelName("Media/tracks/farm/farm_track.b3d");
	Pista->setScale(vector3df(1.0, 1.0, 1.0));

	ObjeScene* Caballo = new ObjeScene();
	Caballo->setModelName("Media/tracks/farm/horse.b3d");
	Caballo->setIdName("Horse1");
	Caballo->setPosition(vector3df(68.93f, 4.59f, 85.82f));
	Caballo->setRotation(vector3df(0.1f, -19.4f, -0.6f));
	Caballo->setScale(vector3df(0.75f, 0.75f, 0.75f));

	//Inicio el Server
	MgrNetwork = new NetworkManagerSr();

	printf("Iniciando Server : %d\n", Port);
	if(!MgrNetwork->initServer(Port))
	{
		printf("Problemas abriendo el Server\n");
		return;
	}

	//Cambio el estado del servidor
	MgrNetwork->setState(NetworkManagerSr::NS_ACCEPT_CONNECTIONS);
	printf("Esperando clientes....\n");

	while(true)
	{
		MgrNetwork->update(0);

		if(MgrNetwork->getNumClients() > 0)
		{
			if( MgrNetwork->getState() == NetworkManagerSr::NS_ACCEPT_CONNECTIONS)
				MgrNetwork->setState(NetworkManagerSr::NS_MODEL_LOADING);
			//continue;
		}
		else
			MgrNetwork->setState(NetworkManagerSr::NS_ACCEPT_CONNECTIONS);

		Message* Mensajes;

		//Envio Datos
		switch(MgrNetwork->getState())
		{
		case NetworkManagerSr::NS_MODEL_LOADING:

			//Cargo el Mundo
			Mensajes = new Msgloadmodel(Pista);
			MgrNetwork->broadcastToClients(Mensajes);
			Mensajes->clear();
			delete Mensajes;

			Mensajes = new MsgEndLoad(1);
			MgrNetwork->broadcastToClients(Mensajes);
			Mensajes->clear();
			delete Mensajes;

			//Cargo los Objetos Extras
			Mensajes = new Msgloadmodel(Caballo);
			MgrNetwork->broadcastToClients(Mensajes);
			Mensajes->clear();
			delete Mensajes;

			Mensajes = new MsgEndLoad(2);
			MgrNetwork->broadcastToClients(Mensajes);
			Mensajes->clear();
			delete Mensajes;

			MgrNetwork->setState(NetworkManagerSr::NS_UPDATING);
			break;
		case NetworkManagerSr::NS_UPDATING:
			UpdateMov();
			Caballo->UpdateCoords(Posicion, Rotacion, Escalado);

			//Creo el emnsaje
			Mensajes = new MsgUpdateObj(Caballo);
			MgrNetwork->broadcastToClients(Mensajes);
			Mensajes->clear();
			delete Mensajes;

			break;
		}
	}
	
	return;
}