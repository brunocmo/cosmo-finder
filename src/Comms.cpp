#include "Comms.h"

int hasResponse = 1;

Comms::Comms()
{ 
    sockedDescriptior = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockedDescriptior == -1)
    {
        perror("Falha na criacao do socket");
        exit(EXIT_FAILURE);
    }

    // Adicionar propriedades do servidor
    memset(&servidorAddr, 0, sizeof(servidorAddr));
    servidorAddr.sin_family = AF_INET;
    servidorAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servidorAddr.sin_port = htons(servidorPorta);

    // Se liga com o endereço local do socket
    int rc = bind(sockedDescriptior, (const struct sockaddr *)&servidorAddr, 
        sizeof(servidorAddr));

    if( rc == -1 ){
        perror("Falha no bind");
        close(sockedDescriptior);
        exit(EXIT_FAILURE);
    }

    // Anuncia para aceitar conexões
    int ls = listen(sockedDescriptior, 5);
    if ( ls == -1 ) {
        perror("Falha no listen!");
        close(sockedDescriptior);
        exit(EXIT_FAILURE);
    }

    clienteLength = sizeof(struct sockaddr_in);

    connectionDescriptor  = accept(sockedDescriptior, (struct sockaddr *)&clienteAddr, &clienteLength);
}

void Comms::init()
{
}

void Comms::RunTCPServer()
{
    while(1) {
        // Block chamador até a requisição de conexão chega
        if( connectionDescriptor == -1) {
            perror("Falha no accept!");
        } else {

            for(int i{0}; i<10; i++) {
                unsigned char valorRecebido = 0;

                int n = recv(connectionDescriptor, &valorRecebido, sizeof(valorRecebido), 0);
                if( n == -1 ) {
                    std::cout << "Erro no recebimento da mensagem!" << '\n';
                } 
                else 
                {
                    receiveBuffer[i] = valorRecebido;
                    while( hasResponse )
                    {
                        vTaskDelay( 300 / portTICK_PERIOD_MS );
                    }
                    hasResponse = 1;
                }
            }

            if( !sendBuffer.empty()) {

                sendCommand(sendBuffer);
            
                sendBuffer.clear();
            }


        }
        vTaskDelay( 300 / portTICK_PERIOD_MS );
    }
}

std::string Comms::receiveCommand()
{
    return reinterpret_cast<char*>( receiveBuffer );
}

bool Comms::sendCommand( std::string message )
{
    int n = send( connectionDescriptor, message.c_str(), message.size(), 0 );

    if( n == -1 ) {
        std::cout << "Erro ao enviar msg" << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}