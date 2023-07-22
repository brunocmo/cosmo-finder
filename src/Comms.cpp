#include "Comms.h"

int hasResponse = 1;

Comms::Comms()
{
    hasSendValue = false;
}

void Comms::Init()
{
    sockedDescriptior = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockedDescriptior == -1)
    {
        std::cout << "FALHA NO SOCKET" << '\n';
    }

    // Adicionar propriedades do servidor
    //memset(&servidorAddr, 0, sizeof(servidorAddr));
    servidorAddr.sin_family = AF_INET;
    servidorAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servidorAddr.sin_port = htons(servidorPorta);

    // Se liga com o endereço local do socket
    int rc = bind(sockedDescriptior, (const struct sockaddr *)&servidorAddr, 
        sizeof(servidorAddr));

    if( rc == -1 ){
        std::cout << "FALHA NO BIND" << '\n';
        close(sockedDescriptior);
    }

    std::cout << "bind" << '\n';

    // Anuncia para aceitar conexões
    int ls = listen(sockedDescriptior, 5);
    if ( ls == -1 ) {
        std::cout << "FALHA NO LISTEN" << '\n';
        close(sockedDescriptior);
    }

    clienteLength = sizeof(struct sockaddr_in);

    connectionDescriptor  = accept(sockedDescriptior, (struct sockaddr *)&clienteAddr, &clienteLength);

    if( connectionDescriptor == -1) {
        std::cout << "FALHA NO ACCEPT" << '\n';
    }
}

void Comms::RunTCPServer( void* pTaskInstance )
{
    Comms* pTask = (Comms* ) pTaskInstance;

    ESP_LOGI( "COMMS", "Server Running" );

    while(1) {
        // Block chamador até a requisição de conexão chega
        if( pTask->connectionDescriptor == -1) {
            ESP_LOGI( "COMMS", "Client connection error, trying new connection..." );
            pTask->connectionDescriptor  = accept(pTask->sockedDescriptior, (struct sockaddr *)&pTask->clienteAddr, &pTask->clienteLength);
            if( pTask->connectionDescriptor == -1) {
                ESP_LOGI( "COMMS", "Something went wrong, restarting server..." );
                close(pTask->sockedDescriptior);
                pTask->Init();
            }

        }
        else 
        {
            char valorRecebido[255];

            int n = recv(pTask->connectionDescriptor, valorRecebido, 255, 0);
            
            if( n == -1 )
            {
                ESP_LOGI( "COMMS", "Client connection error, trying new connection..." );

                pTask->connectionDescriptor  = accept(pTask->sockedDescriptior, (struct sockaddr *)&pTask->clienteAddr, &pTask->clienteLength);
                if( pTask->connectionDescriptor == -1) {
                    std::cout << "FALHA NO ACCEPT" << '\n';
                    close(pTask->sockedDescriptior);
                }
            } 
            else 
            {
                pTask->sizeOfBuffer = n;
                //  std::cout << "Recebi: " << n << " mensagens" << '\n';

                if( n == 0 ) continue;

                // for( int i{0}; i<n; i++)
                // {
                //     std::cout << std::hex << (int)valorRecebido[i] << " ";
                // }
                // std::cout << '\n';

                memcpy( pTask->receiveBuffer , valorRecebido, 255 );

                while( !pTask->hasSendValue )
                {
                    vTaskDelay( 300 / portTICK_PERIOD_MS );
                }

                int n = send( pTask->connectionDescriptor, pTask->sendBuffer, 28, 0 );

                if( n == -1 ) {
                    std::cout << "Erro ao enviar msg" << '\n';
                }
            
                pTask->hasSendValue = false;
            }
        }
        vTaskDelay( 300 / portTICK_PERIOD_MS );
    }
}

char* Comms::receiveCommand()
{
    memcpy( tempReceiveBuffer, receiveBuffer, 255 );
    memset( receiveBuffer, 0, 255 );
    return tempReceiveBuffer;
}

void Comms::sendCommand( const char* message )
{
    strcpy( sendBuffer, message );
    hasSendValue = true;
}