#include <QCoreApplication>
#include "asio/AsioServerDuplex.h"
#include "../../Common.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  spo::asio::AsioServerDuplex< spo::asio::AsioTCPServer< char >, char>
      server( 33333, 33444, 1000 );

  server.SetSocketDeadline( spo::asio::DataType::Input  , 1000 );
  server.SetSocketDeadline( spo::asio::DataType::Output , 2000 );

  server.SetBufferAction(
        spo::asio::DataType::Input,
        boost::bind( DumpReceive, _1 ) );

  server.SetBufferAction(
        spo::asio::DataType::Output,
        boost::bind( DumpSend, _1 ) );

  while( not spo::asio::AsioService::Instance().Start() );
  while(true);
//  server.Start();
  return a.exec();
}
