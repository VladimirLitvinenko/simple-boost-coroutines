#include <QCoreApplication>
#include "asio/AsioTCPServer.h"
#include "../../Common.h"

#undef ACCEPT_IN_WHILE
//#define ACCEPT_IN_WHILE

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  SetHalfDuplexMode( true );

  spo::asio::AsioTCPServer<char> server( spo::asio::TransferType::HalfDuplexIn, 33444 );

  server.SetSocketDeadline( 10000 );
  server.SetBufferAction(
        spo::asio::DataType::Input,
        boost::bind( DumpReceive, _1 ) );

  server.SetBufferAction(
        spo::asio::DataType::Output,
        boost::bind( DumpSend, _1 ) );

  while( not spo::asio::AsioService::Instance().Start() );

  return a.exec();
}
