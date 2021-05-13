#ifndef ASIOCLIENT_H
#define ASIOCLIENT_H


#include "asio/AsioResolver.h"
#include "asio/ClientServerBase.h"
#include "asio/AsioSocketSession.h"

namespace                         spo   {
namespace                         asio  {

//------------------------------------------------------------------------------
/**
 * @brief Класс AsioClient
 */
template
<
    typename                      ProtocolT_,
    typename                      ByteT_      = unsigned char,
    typename                    = typename std::enable_if
    <
      std::is_same< ProtocolT_, boost::asio::ip::tcp >::value
      or
      std::is_same< ProtocolT_, boost::asio::ip::udp >::value
      or
      std::is_same< ProtocolT_, boost::asio::ip::icmp >::value
    >::type
>
class SPO_CORE_EXPORT             AsioClient :
public                            spo::asio::ClientServerBase< ProtocolT_, ByteT_ >,
public                            spo::asio::AsioResolver< ProtocolT_, ByteT_ >
{
public:
  using self_t                  = spo::asio::AsioClient< ProtocolT_, ByteT_ >;
  using base_class_t            = spo::asio::ClientServerBase< ProtocolT_, ByteT_ >;
  using resolver_t              = spo::asio::AsioResolver< ProtocolT_, ByteT_ >;
  using session_t               = spo::asio::AsioSocketSession< ProtocolT_, ByteT_ >;
  using session_shr_t           = std::shared_ptr< typename self_t::session_t >;

private:
  std::atomic_bool              m_KeepAlive { false };

public:
  /**
   * @brief Метод SetBeforeServiceStart
   * @param type
   */
  void SetBeforeServiceStart () BOOST_NOEXCEPT
  {
    spo::asio::AsioService::Instance().AddBeforeStartCallback
        ( {
            []( void * ptr)
            {
              if( nullptr != ptr )
              {
                reinterpret_cast< self_t* >(ptr)->TryConnect();
                if( not IsNoErr( spo::asio::AsioService::Instance().ErrorCode() ) )
                {
                  spo::asio::AsioService::Instance().Stop();
                }
              }
            },
            this
          } );
  }

  /**
   * @brief AsioClient
   * @param type
   * @param host
   * @param remoute_service
   */
  explicit AsioClient
  (
      spo::asio::TransferType         type,
      const std::string             & host,
      const std::string             & remoute_service,
      bool                            keepAlive,
      std::int64_t                    serviceTimeoutMs = 10000
 ) : base_class_t ( type, serviceTimeoutMs )
   , resolver_t   ( host, remoute_service, serviceTimeoutMs )
  {
    SetBeforeServiceStart();
    SetKeepAlive ( keepAlive );
  }

  bool IsKeepAlive () const
  {
    return m_KeepAlive.load();
  }

  void SetKeepAlive ( const bool value )
  {
    m_KeepAlive.store( value );
    base_class_t::ClearSessionAfterStop();

    if( IsKeepAlive() )
    {
      base_class_t::SetSessionAfterStop(
            [ this ]( void * ptr )
            {
              auto cl_ptr ( reinterpret_cast< spo::asio::AsioClient< ProtocolT_, ByteT_ > * >(ptr) );
              if( nullptr != cl_ptr )
              {
                cl_ptr->DecSocketsCount();
              }
            },
            this );
    }
  }

  /**
   * @brief Метод TryConnect производит попытки подключения к серверу
   * @param type
   */
  void TryConnect ()
  {
    if( not base_class_t::SocketsValid() )
    {
      AsioService::Instance().SetState( AsioState::ErrSocketCount );
      return;
    }

    if( resolver_t::IsValid( true ) )
    {
      try
      {
        boost::asio::spawn(
              io_strand_t( self_t::ServiceRef() ),
              boost::bind( & self_t::Connect, this, _1 ) );
      }
      catch ( const std::exception & e)
      {
        DUMP_EXCEPTION( e );
      }
    }
  }

protected:
  /**
   * @brief Метод Connect реализует функционал подключения к серверу.
   * @param type тип (режим) работы сервера
   * @param yield контекст передачи управления очередной сопрограмме
   */
  void Connect( boost::asio::yield_context yield )
  {
    try
    {
      boost::system::error_code ec;
      typename ProtocolT_::socket socket( resolver_t::ServiceRef() );
      base_class_t::IncSocketsCount();

      while( IsKeepAlive() )
      for( auto ep : self_t::Endpoints() )
      {
        socket.async_connect( ep, yield[ ec ] );

        if( spo::asio::AsioService::Instance().IsError( ec ) )
        {
          AsioService::Instance().SetState( AsioState::ErrConnection );
          base_class_t::DecSocketsCount();
          break;
        }
        // создание сессии работы с сокетом
        auto session_ptr( std::move( MakeSocketSession< ProtocolT_, ByteT_ >(
                                      self_t::ActionsRef(),
                                      base_class_t::TransferType(),
                                      std::move( socket ),
                                      ep,
                                      self_t::SocketDeadline() ) ) );
        if( session_ptr )
        {
          // сессия создана успешно, запуск транзакции работы с данными
          session_ptr->SetAfterStop( base_class_t::SessionAfterStop(),
                                     base_class_t::SessionAfterStopParam() );

          self_t::ServiceRef().post(boost::bind( & session_t::Start, session_ptr ) );
          break;
        }
        spo::asio::AsioService::Instance().SetError( boost::system::errc::owner_dead );
      }
    }
    catch( std::exception & e )
    {
      DUMP_EXCEPTION( e );
    }
  }
};

//------------------------------------------------------------------------------

}// namespace                     asio
}// namespace                     spo

#endif // ASIOCLIENT_H
