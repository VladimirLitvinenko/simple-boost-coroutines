#include <QCoreApplication>
#include "asio/AsioClient.h"
#include "../../Common.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  using namespace spo::asio;

  UNUSED( spo::asio::AsioService::Instance( 2000 ) );

  SetHalfDuplexMode( true );

  auto  connector ( std::make_shared< spo::asio::AsioClient< boost::asio::ip::tcp, char > >
                    ( spo::asio::TransferType::SimplexIn,
                      "127.0.0.1",
                      "33444",
                      1000 ) );


  spo::asio::AsioService::Instance().AddAfterStopCallback
      ( {
          []( void * )
          {
            std::cout << "SERVICE STOPED...." << std::endl;
          }
          ,
          nullptr
        } );

  connector->SetTimeoutMs( 200 );
  connector->SetBufferAction(
        spo::asio::DataType::Input,
        boost::bind( DumpReceive, _1 ) );

  connector->SetBufferAction(
        spo::asio::DataType::Output,
        boost::bind( DumpSend, _1 ) );

//  while( true  )
//  {
//    UNUSED( spo::asio::AsioService::Instance().Start() );
//  }
  while( not spo::asio::AsioService::Instance().Start() );

  return a.exec();
}
