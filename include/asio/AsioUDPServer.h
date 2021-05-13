#ifndef ASIOUDPSERVER_H
#define ASIOUDPSERVER_H

#include "asio/AsioServer.h"
#include "asio/AsioSocketSession.h"

namespace                         spo   {
namespace                         asio  {

//------------------------------------------------------------------------------

template< typename                ByteT_ >
class SPO_CORE_EXPORT             AsioUDPServer :
public                            spo::asio::AsioServer< boost::asio::ip::udp, ByteT_ >
{
public:
  using self_t                  = spo::asio::AsioUDPServer< ByteT_ >;
  using udp_t                   = boost::asio::ip::udp;
  using base_class_t            = spo::asio::AsioServer< self_t::udp_t, ByteT_ >;

  boost::system::error_code TryOpen( boost::asio::ip::udp::socket & socket )
  {
    boost::system::error_code ec =
        boost::system::errc::make_error_code( boost::system::errc::success );
    try
    {
      if( not socket.is_open() )
      {
        socket.open( base_class_t::Protocol(), ec );
        spo::asio::SetSocketOptions< boost::asio::ip::udp >( socket );

        AsioService::Instance().SetError( ec );
        if( IsNoErr( ec ) )
        {
          socket.bind( base_class_t::Endpoint(), ec );
          AsioService::Instance().SetError( ec );
        }
      }
    }
    catch( const std::exception & e )
    {
      AsioService::Instance().SetError( boost::system::errc::owner_dead );
      DUMP_EXCEPTION( e );
    }

    if( AsioService::Instance().IsError( ec ) )
    {
      socket.shutdown( boost::asio::socket_base::shutdown_both, ec );
      socket.close( ec );
    }
    return AsioService::Instance().ErrorCode();
  }

  /**
   * @brief Конструктор AsioUDPServer формирует экземпляр класса для
   *        прослушивания UDP-подключений по сети к серверу по назначенному
   *        порту.
   * @param port порт UDP-сервера
   * @param protocol протокол для передачи данных. По умолчанию -
   */
  /**/                            AsioUDPServer
  (
      const spo::asio::TransferType   type,
      const spo::asio::port_t         port,
      const std::int64_t            & serviceTimeoutMs = 10000
  )
    : self_t::base_class_t( type, port, self_t::udp_t::v4(), serviceTimeoutMs )
  {
    Init();
  }

private:

  /**
   * @brief Метод Listen реализует алгоритм обработки UDP подключений
   * @param yield контекстный метод обработки передачи управления сопрограмме
   */
  void Listen ( /*boost::asio::ip::udp::socket && socket, */boost::asio::yield_context yield )
  {
    spo::asio::error_t ec;
    if( not base_class_t::SocketsValid() )
    {
      AsioService::Instance().SetState( AsioState::ErrSocketCount );
      yield[ ec ];
      return;
    }

    // назначение сокета и проверка подключения
    boost::asio::ip::udp::socket socket( AsioService::Instance().ServiceRef() );

    ec = TryOpen( socket );

    if( IsNoErr( ec ) )
    {
      base_class_t::IncSocketsCount();

      // создание shared-сессии работы с UDP-сокетом
      auto session_ptr( std::move( MakeSocketSession< boost::asio::ip::udp, ByteT_ >(
                                    base_class_t::ActionsRef(),
                                    base_class_t::TransferType(),
                                    std::move( socket ),
                                    base_class_t::Endpoint(),
                                    base_class_t::SocketDeadline() ) ) );
      if( session_ptr )
      {
        struct socket_udp
        {
          boost::asio::ip::udp::socket && s;
          spo::asio::AsioUDPServer< ByteT_ > * ptr = this;
          socket_udp( boost::asio::ip::udp::socket &&sk, spo::asio::AsioUDPServer< ByteT_ > * p )
            : s( std::move( sk ) )
            , ptr( p )
          {}
        };
        socket_udp obj( std::move( socket ), this );

        session_ptr->SetAfterStop(
              [ this, yield ]( void * ptr )
              {
                auto s_ptr( reinterpret_cast< socket_udp * >(ptr) );
                if( nullptr != s_ptr )
                {
                  s_ptr->ptr->DecSocketsCount();
                  //перезапуск UDP-приема после останова работы сессии
//                  if( AsioService::Instance().IsActive() and s_ptr->ptr->SocketsValid() )
//                  {
//                    try
//                    {
//                      std::async( std::launch::async,
//                                  [ this, yield, s_ptr]
//                      {
//                      boost::asio::spawn(
//                            io_strand_t( AsioService::Instance().ServiceRef() ),
////                            boost::bind( & self_t::Listen, s_ptr, yield ) );
//                            boost::bind( & self_t::Listen,
//                                         s_ptr->ptr,
//                                         s_ptr->s,
//                                         yield ) );
//                      } ).wait_for( std::chrono::milliseconds( 0 ) );
//                    }
//                    catch( const std::exception & e )
//                    {
//                      DUMP_EXCEPTION( e )
//                    }
//                  }
                }
              }, &obj );
        AsioService::Instance().ServiceRef().post(
          boost::bind(
            & spo::asio::AsioSocketSession< boost::asio::ip::udp, ByteT_ >::Start,
            session_ptr ) );
      }
      else
      {
        ec = boost::system::errc::make_error_code( boost::system::errc::invalid_argument );
      }
    }
  }

  /**
   * @brief Метод Init устанавливает алгорим прослушивания UDP-подклучений после
   *        запуска сервиса обработкм ввода/вывода типа @a boost::asio::io_service
   *
   * @see AsioService::Instance
   */
  void Init () BOOST_NOEXCEPT
  {
    AsioService::Instance().AddBeforeStartCallback
        ( {
            [ this ](void * ptr)
            {
              auto self( reinterpret_cast< self_t * >(ptr) );
              if( nullptr != self )
              {
                try
                {
                  boost::asio::spawn(
                        io_strand_t( AsioService::Instance().ServiceRef() ),
                        boost::bind(& self_t::Listen,
                                      self,
//                                      std::move( boost::asio::ip::udp::socket( AsioService::Instance().ServiceRef() ) ),
                                      _1 ) );
                }
                catch ( const std::exception & e)
                {
                  DUMP_EXCEPTION( e );
                }
              }
            },
            this
          } );
  }
};

//------------------------------------------------------------------------------

}// namespace                     asio
}// namespace                     spo


#endif // ASIOUDPSERVER_H
