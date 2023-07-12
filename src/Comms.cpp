#include "Comms.h"

int hasResponse = 1;

Comms::Comms()
{ 

    std::cout << "INICIEI" << '\n';
}

void Comms::Init()
{
    std::cout << "DE VERDADE" << '\n';    
    sockedDescriptior = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockedDescriptior == -1)
    {
        std::cout << "FALHA NO SOCKET" << '\n';
    }

    std::cout << "socket" << '\n';

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

    std::cout << "listen" << '\n';

    clienteLength = sizeof(struct sockaddr_in);

    connectionDescriptor  = accept(sockedDescriptior, (struct sockaddr *)&clienteAddr, &clienteLength);

    if( connectionDescriptor == -1) {
        std::cout << "FALHA NO ACCEPT" << '\n';
    }

    std::cout << "INICEI BEM PAI" << '\n';

}

void Comms::RunTCPServer( void* pTaskInstance )
{
    Comms* pTask = (Comms* ) pTaskInstance;

    std::cout << "Rodando o servidor!!!" << '\n';

    while(1) {
        // Block chamador até a requisição de conexão chega
        if( pTask->connectionDescriptor == -1) {
            std::cout << "FALHA NO ACCEPT fechado" << '\n';
            pTask->connectionDescriptor  = accept(pTask->sockedDescriptior, (struct sockaddr *)&pTask->clienteAddr, &pTask->clienteLength);
            if( pTask->connectionDescriptor == -1) {
                std::cout << "FALHA NO ACCEPT" << '\n';
                close(pTask->sockedDescriptior);
            }
            pTask->Init();
        }
        else 
        {
            char valorRecebido[255];

            int n = recv(pTask->connectionDescriptor, valorRecebido, 255, 0);
            
            if( n == -1 )
            {
                std::cout << "Erro no recebimento da mensagem!" << '\n';
                pTask->connectionDescriptor  = accept(pTask->sockedDescriptior, (struct sockaddr *)&pTask->clienteAddr, &pTask->clienteLength);
                if( pTask->connectionDescriptor == -1) {
                    std::cout << "FALHA NO ACCEPT" << '\n';
                    close(pTask->sockedDescriptior);
                }
            } 
            else 
            {
                pTask->sizeOfBuffer = n;
                std::cout << "Recebi: " << n << " mensagens" << '\n';

                if( n == 0 ) continue;

                // for( int i{0}; i<n; i++)
                // {
                //     std::cout << std::hex << (int)valorRecebido[i] << " ";
                // }
                // std::cout << '\n';

                memcpy( pTask->receiveBuffer , valorRecebido, 255 );

                while( pTask->sendBuffer.empty() )
                {
                    vTaskDelay( 300 / portTICK_PERIOD_MS );
                }

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

char* Comms::receiveCommand()
{
    memcpy( tempReceiveBuffer, receiveBuffer, 255 );
    memset( receiveBuffer, 0, 255 );
    return tempReceiveBuffer;
}

void Comms::sendCommand( std::string message )
{
    sendBuffer = message;
}