#ifndef ASIOSERVER_H
#define ASIOSERVER_H

#include "asio/ClientServerBase.h"
#include "asio/AsioSocketSession.h"

namespace                         spo   {
namespace                         asio  {

//------------------------------------------------------------------------------

template
<
    typename                      ProtocolT_,
    typename                      ByteT_,
    typename                    = typename std::enable_if
    <
      std::is_same< ProtocolT_, boost::asio::ip::tcp >::value
      or
      std::is_same< ProtocolT_, boost::asio::ip::udp >::value
      or
      std::is_same< ProtocolT_, boost::asio::ip::icmp >::value
    >::type
>
class SPO_CORE_EXPORT             AsioServer :
public                            spo::asio::ClientServerBase< ProtocolT_, ByteT_ >
{
public:
  using self_t                  = spo::asio::AsioServer< ProtocolT_, ByteT_ >;
  using base_class_t            = spo::asio::ClientServerBase< ProtocolT_, ByteT_ >;
  using protocol_t              = ProtocolT_;

private:
  endpoint_t< ProtocolT_ >        m_Endpoint;

public:
  /**
   * @brief Конструктор AsioServer формирует экземпляр класса для
   *        прослушивания подключений по сети по назначенному
   *        порту и типу протокола.
   * @param ports порты сервера
   * @param protocol протокол для передачи данных.
   */
  /**/                            AsioServer
  (
      const spo::asio::TransferType   type,
      const spo::asio::port_t         port,
      const ProtocolT_                protocol_version,
      const std::int64_t            & serviceTimeoutMs = 10000
  )
    : base_class_t  ( type, serviceTimeoutMs )
    , m_Endpoint    ( spo::asio::endpoint_t< ProtocolT_ >(
                            const_cast< ProtocolT_ & >( protocol_version ),
                            port ) )
  {
    spo::asio::AsioService::Instance().
        SetState( Port() > 0
                  ? AsioState::Ok
                  : AsioState::ErrPortCount );
  }

  /**
   * @brief Метод Protocol возвращает тип (версию) подключения:
   * @return Значение типа (версии) подключения
   */
  ProtocolT_ Protocol() const
  {
    return m_Endpoint.protocol();
  }

  /**
   * @brief Метод Endpoint возвращает экземпляр класcа точки подключения к серверу.
   * @return Значение точки подключения к серверу.
   */
  decltype( m_Endpoint ) Endpoint() const
  {
    return m_Endpoint;
  }

  /**
   * @brief метод Port сообщает значение порта, открытого сервером для приема подключений.
   * @return значение порта, открытого сервером для приема подключений.
   */
  spo::asio::port_t Port () const
  {
    return static_cast< spo::asio::port_t >( m_Endpoint.port() );
  }

  /**
   * @brief Метод Start запускает IP-сервер на обслуживание подключений сетевых
   *        клинтов.
   * @return Булево значение:
   * @value true сообщает об успешном запуске сервера;
   * @value false сообщает о неудачном запуске.
   */
  bool Start () BOOST_NOEXCEPT
  {
    return
        base_class_t::IsValid()
        and
        ( self_t::service_t::Instance().IsActive()
          ? true
          : self_t::service_t::Instance().Start() );
  }

  /**
   * @brief Метод Stop останавливает работу сервера по обслуживанию подключений/
   * @return Булево значение:
   * @value true сообщает об успешном останове сервера;
   * @value false сообщает о останове запуске.
   */
  bool Stop () BOOST_NOEXCEPT
  {
    return
        base_class_t::IsValid()
        and
        self_t::service_t::Instance().Stop();
  }

  /**
   * @brief Метод TemporaryStart временно запускает сервер на обслуживание
   *        подключений сетевых  клинтов.
   * @param timeoutMs время в миллисекундах работы сервера.
   * @return Булево значение:
   * @value true сообщает об успешном запуске сервера;
   * @value false сообщает о неудачном запуске.
   */
  bool TemporaryStart ( int64_t timeoutMs ) BOOST_NOEXCEPT
  {
    bool retval( Start() );
    if( retval )
    {
      std::this_thread::sleep_for( std::chrono::milliseconds( timeoutMs ) );
      retval = Stop();
    }
    return retval;
  }
};

//------------------------------------------------------------------------------

}// namespace                     asio
}// namespace                     spo


#endif // ASIOSERVER_H
