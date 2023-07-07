#include "Comms.h"

int hasResponse = 1;

Comms::Comms()
{ 
}

void Comms::Init()
{
    sockedDescriptior = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockedDescriptior == -1)
    {
        perror("Falha na criacao do socket");
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
    }

    // Anuncia para aceitar conexões
    int ls = listen(sockedDescriptior, 5);
    if ( ls == -1 ) {
        perror("Falha no listen!");
        close(sockedDescriptior);
    }

    clienteLength = sizeof(struct sockaddr_in);

    connectionDescriptor  = accept(sockedDescriptior, (struct sockaddr *)&clienteAddr, &clienteLength);
}

void Comms::RunTCPServer( void* pTaskInstance )
{
    Comms* pTask = (Comms* ) pTaskInstance;

    while(1) {
        // Block chamador até a requisição de conexão chega
        if( pTask->connectionDescriptor == -1) {
            perror("Falha no accept!");
        } else {
            unsigned char valorRecebido[256];

            int n = recv(pTask->connectionDescriptor, valorRecebido, 256, 0);
            
            if( n == -1 )
            {
                std::cout << "Erro no recebimento da mensagem!" << '\n';
            } 
            else 
            {
                pTask->receiveBuffer = std::string( (char* )valorRecebido );
                while( hasResponse )
                {
                    vTaskDelay( 300 / portTICK_PERIOD_MS );
                }
                hasResponse = 1;
            }


            if( !pTask->sendBuffer.empty() )
            {
                int n = send( pTask->connectionDescriptor, pTask->sendBuffer.c_str(), pTask->sendBuffer.size(), 0 );

                if( n == -1 ) {
                    std::cout << "Erro ao enviar msg" << '\n';
                }
            
                pTask->sendBuffer.clear();
            }


        }
        vTaskDelay( 300 / portTICK_PERIOD_MS );
    }
}

std::string Comms::receiveCommand()
{
    return receiveBuffer;
}

void Comms::sendCommand( std::string message )
{
    sendBuffer = message;
}