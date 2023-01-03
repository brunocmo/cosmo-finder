#include <string>
#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "wifi.h"

#define WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD

class Main final
{
private:
public:
    void run(void);
    void setup(void);
	void testServer(void);

    WIFI::Wifi::state_e wifiState { WIFI::Wifi::state_e::NOT_INITIALIZED };
    WIFI::Wifi Wifi;
};

Main App;

void Main::run(void)
{
    wifiState = Wifi.GetState();

    switch (wifiState)
    {
    case WIFI::Wifi::state_e::READY_TO_CONNECT:
        std::cout << "Wifi Status: READY_TO_CONNECT\n";
        Wifi.Begin();
        break;
    case WIFI::Wifi::state_e::DISCONNECTED:
        std::cout << "Wifi Status: DISCONNECTED\n";
        Wifi.Begin();
        break;
    case WIFI::Wifi::state_e::CONNECTING:
        std::cout << "Wifi Status: CONNECTING\n";
        break;
    case WIFI::Wifi::state_e::WAITING_FOR_IP:
        std::cout << "Wifi Status: WAITING_FOR_IP\n";
        break;
    case WIFI::Wifi::state_e::ERROR:
        std::cout << "Wifi Status: ERROR\n";
        break;
    case WIFI::Wifi::state_e::CONNECTED:
        std::cout << "Wifi Status: CONNECTED\n";
		// testServer();
        break;
    case WIFI::Wifi::state_e::NOT_INITIALIZED:
        std::cout << "Wifi Status: NOT_INITIALIZED\n";
        break;
    case WIFI::Wifi::state_e::INITIALIZED:
        std::cout << "Wifi Status: INITIALIZED\n";
        break;
    }
}

void Main::testServer(void) {
	int clienteSocket;
	struct sockaddr_in servidorAddr;
	unsigned short servidorPorta = 16188;
	const char *IP_Servidor = "192.168.0.173";
	const char *mensagem = "Meih Fuhrer, I CAN WALK!";
	char buffer[16];
	unsigned int tamanhoMensagem;

	int bytesRecebidos;
	int totalBytesRecebidos;

	totalBytesRecebidos = 0;
	while(1) {

		// Criar Socket
		if((clienteSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
			printf("Erro no socket()\n");

		// Construir struct sockaddr_in
		memset(&servidorAddr, 0, sizeof(servidorAddr)); // Zerando a estrutura de dados
		servidorAddr.sin_family = AF_INET;
		servidorAddr.sin_addr.s_addr = inet_addr(IP_Servidor);
		servidorAddr.sin_port = htons(servidorPorta);

		// Connect
		if(connect(clienteSocket, (struct sockaddr *) &servidorAddr, 
								sizeof(servidorAddr)) < 0)
			printf("Erro no connect()\n");

		tamanhoMensagem = strlen(mensagem);

		if(send(clienteSocket, mensagem, tamanhoMensagem, 0) != tamanhoMensagem)
			printf("Erro no envio: numero de bytes enviados diferente do esperado\n");

		if((bytesRecebidos = recv(clienteSocket, buffer, 16-1, 0)) <= 0)
			printf("Não recebeu o total de bytes enviados\n");
		totalBytesRecebidos += bytesRecebidos;
		buffer[bytesRecebidos] = '\0';
		printf("%s\n", buffer);

		close(clienteSocket);

        vTaskDelay(pdMS_TO_TICKS(1000));
	}


}

void Main::setup(void)
{
    esp_event_loop_create_default();
    nvs_flash_init();

    Wifi.SetCredentials(WIFI_SSID, WIFI_PASS);
    Wifi.Init();
}

extern "C" void app_main(void)
{
    App.setup();
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        App.run();
    }
}