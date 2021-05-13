#include <QCoreApplication>
#include "asio/AsioTCPServer.h"
#include "../../Common.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  spo::asio::AsioTCPServer<char> server( spo::asio::TransferType::SimplexOut, 33444 );
  server.SetSocketDeadline( 2000 );

  server.SetBufferAction(
        spo::asio::DataType::Output,
        boost::bind( DumpSend, _1 ) );

  while( not spo::asio::AsioService::Instance().Start() );

  return a.exec();
}
