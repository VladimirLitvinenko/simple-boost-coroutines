#include <QCoreApplication>
#include "asio/AsioTCPServer.h"
#include "../../Common.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  spo::asio::AsioTCPServer<char> server( spo::asio::TransferType::SimplexIn, 33333 );
  server.SetSocketDeadline( 2000 );
  server.SetBufferAction(
        spo::asio::DataType::Input,
        boost::bind( DumpReceive, _1 ) );

  while( not spo::asio::AsioService::Instance().Start() );

  return a.exec();
}
