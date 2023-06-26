#pragma once
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>

class NetworkManager
{
private:
	enum NetworkState
	{
		WAITING_FOR_CREDENTIALS,
		CONNECTED_TO_WIFI,
		CONNECTED_TO_HOST
	};

	NetworkState _curState;
	
	//Broadcast
	static const int UDP_PACKET_SIZE = 48;
	char _udpSendBuffer[UDP_PACKET_SIZE];
	static const uint16_t BROADCAST_PORT = 6678;
	static const uint16_t BROADCAST_DELAY_MS = 1000;

	bool _ledToggle = false;
	
	uint16_t _updateTimer = 0;
	uint32_t _lastUpdatetime = 0;

	IPAddress _localIP;
	IPAddress _remoteIP;

	//UDP
	WiFiUDP _Udp;
	IPAddress _broadcastAdress;

	//Server
	ESP8266WebServer _webServer;
	WiFiClient _wifiClient;

	void InitSendBuffer();
	void CheckUDPResponse();
	void SendUDPBroadcast();
	//If ssid is not specified (empty string) mcu tries to connect to last network
	void TryConnectToNetwork(const char* ssid, const char* pw);
	///Webserver stuff
	void BeginWebConfig();
	void handleRoot();
	void handleLogin();
	void handleNotFound();
public:	
	NetworkManager();
	~NetworkManager();

	void Begin();
	void Update();
	//Forces to send data, even if buffer isn't full yet
	bool Flush();
};

