#ifndef ASIOSERVERDUPLEX_H
#define ASIOSERVERDUPLEX_H

#include "asio/AsioTCPServer.h"
#include "asio/AsioUDPServer.h"

namespace                         spo   {
namespace                         asio  {

//------------------------------------------------------------------------------
template
<
    typename                      ServerType_,
    typename                      ByteT_,
    typename                    = typename std::enable_if
    <
      std::is_same< ServerType_, spo::asio::AsioTCPServer< ByteT_ > >::value
      or
      std::is_same< ServerType_, spo::asio::AsioUDPServer< ByteT_ > >::value
    >::type
>
class SPO_CORE_EXPORT             AsioServerDuplex
{
public:
  using self_t                  = spo::asio::AsioServerDuplex< ServerType_, ByteT_ >;
  using server_t                = ServerType_;
  using server_shr_t            = std::shared_ptr< server_t >;
  using servers_t               = std::vector< server_shr_t >;

private:
  mutable servers_t               m_Servers;

public:
  /**
   * @brief Конструктор AsioServer формирует экземпляр класса для
   *        прослушивания подключений по сети по назначенному
   *        порту и типу протокола.
   * @param ports порты сервера
   * @param protocol протокол для передачи данных.
   */
  /**/                            AsioServerDuplex
  (
      const duplex_ports_t      & ports,
      const std::int64_t        & serviceTimeoutMs = 10000
  )
  {
    assert( not ports.empty() );
    std::size_t p_size( ports.size() );
    assert( 2 == p_size );
    m_Servers.resize( p_size );

    using idx_type = std::pair< std::size_t, spo::asio::TransferType >;

    for( auto idx :
    {
         idx_type{ 0, spo::asio::TransferType::SimplexIn  },
         idx_type{ 1, spo::asio::TransferType::SimplexOut },
    } )
    {
      m_Servers.at( idx.first ) =
            std::make_shared< ServerType_ >( idx.second,
                                             ports.at( idx.first ),
                                             serviceTimeoutMs );
    }
  }

  /**
   * @brief Конструктор AsioServer формирует экземпляр класса для
   *        прослушивания подключений по сети по назначенному
   *        порту и типу протокола.
   * @param portsRx порт приема данных на сервер;
   * @param portsTx порт передачи данных от сервера.
   * @param protocol протокол для передачи данных.
   */
  /**/                            AsioServerDuplex
  (
      const spo::asio::port_t   & portsRx,
      const spo::asio::port_t   & portsTx,
      const std::int64_t        & serviceTimeoutMs = 10000
  ) :
    AsioServerDuplex( duplex_ports_t( { portsRx, portsTx } ), serviceTimeoutMs )
  {}

  /**
   * @brief Метод ResetState
   */
  void ResetState () BOOST_NOEXCEPT
  {
    for( auto & srv_ref : m_Servers )
    {
      srv_ref->ResetState();
    }
  }

  /**
   * @brief Метод IsValid
   * @return
   */
  bool IsValid () const BOOST_NOEXCEPT
  {
    bool retval( not m_Servers.empty() );
    for( auto & srv_ref : m_Servers )
    {
      retval = retval and srv_ref->IsValid();
    }
    return retval;
  }

  /**
   * @brief Метод IsActive сообщает об активном состоянии сервиса
   *        типа @a boost::asio::io_service обработки действий над сокетами
   * (объектами типа @a boost::asio::ip::tcp::socket).
   * @return Булево значение:
   * @value true сообщает об активном состоянии сервиса;
   * @value false сообщает об НЕактивном состоянии сервиса.
   */
  bool IsActive () const BOOST_NOEXCEPT
  {
    bool retval( not m_Servers.empty() );
    for( auto & srv_ref : m_Servers )
    {
      retval = retval and srv_ref->IsActiive();
    }
    return retval;
  }

  /**
   * @brief Метод IsOpen
   * @return
   */
  bool IsOpen () const BOOST_NOEXCEPT
  {
    bool retval( not m_Servers.empty() );
    for( auto & srv_ref : m_Servers )
    {
      retval = retval and srv_ref->IsOpen();
    }
    return retval;
  }

  /**
   * @brief SocketDeadline
   * @param dt
   * @return
   */
  boost::int64_t SocketDeadline ( const spo::asio::DataType & type )
  {
    std::size_t idx( type == spo::asio::DataType::Input ? 0 : 1 );
    assert( idx < m_Servers.size() );
    return
        IsValid()
        ? m_Servers.at( idx )->SocketDeadline()
        : -1;
  }

  /**
   * @brief SetSocketDeadline
   * @param socketDeadline
   */
  void SetSocketDeadline( const spo::asio::DataType & type , const boost::int64_t & socketDeadline )
  {
    std::size_t idx( type == spo::asio::DataType::Input ? 0 : 1 );
    assert( idx < m_Servers.size() );
    if( IsValid() )
    {
      m_Servers.at( idx )->SetSocketDeadline( socketDeadline );
    }
  }

  /**
   * @brief Метод Port возвращает значение порта, на котором сервер принимает
   *        подключения.
   * @return значение порта.
   */
  spo::asio::port_t Port ( const spo::asio::DataType & type ) const
  {
    std::size_t idx( type == spo::asio::DataType::Input ? 0 : 1 );
    assert( idx < m_Servers.size() );
    return
        IsValid()
        ? m_Servers.at( idx )->Port()
        : -1;
  }

  /**
   * @brief SetBufferAction
   * @param key
   * @param action
   */
  void SetBufferAction
  (
      const spo::asio::DataType               & key,
      spo::asio::io_channel_action_t< ByteT_ >  action
  )
  {
    assert( key < spo::asio::DataType::DataSize );
    if( IsValid() )
    {
      m_Servers.at( key == spo::asio::DataType::Input ? 0 : 1 )->SetBufferAction( key, action );
    }
  }

  /**
   * @brief TimeoutMs
   * @return
   */
  std::int64_t TimeoutMs ( const spo::asio::DataType & type ) const
  {
    std::size_t idx( type == spo::asio::DataType::Input ? 0 : 1 );
    assert( idx < m_Servers.size() );
    return
        IsValid()
        ? m_Servers.at( idx )->TimeoutMs()
        : -1;
  }

  /**
   * @brief SetTimeoutMs
   * @param timeoutMs
   */
  void SetTimeoutMs ( const spo::asio::DataType & type, const std::int64_t & timeoutMs )
  {
    std::size_t idx( type == spo::asio::DataType::Input ? 0 : 1 );
    assert( idx < m_Servers.size() );
    if( IsValid() )
    {
      m_Servers.at( idx )->SetTimeoutMs( timeoutMs );
    }
  }

  /**
   * @brief SocketsLimit
   * @return
   */
  int SocketsLimit ( const spo::asio::DataType & type ) const
  {
    std::size_t idx( type == spo::asio::DataType::Input ? 0 : 1 );
    assert( idx < m_Servers.size() );
    return
        IsValid()
        ? m_Servers.at( idx )->SocketsLimit()
        : -1;
  }

  /**
   * @brief SetSocketsLimit
   * @param socketsLimit
   */
  void SetSocketsLimit ( const spo::asio::DataType & type, int socketsLimit )
  {
    std::size_t idx( type == spo::asio::DataType::Input ? 0 : 1 );
    assert( idx < m_Servers.size() );
    if( IsValid() )
    {
      m_Servers.at( idx )->SetSocketsLimit( socketsLimit );
    }
  }

  /**
   * @brief SocketsValid
   * @return
   */
  bool SocketsValid ( const spo::asio::DataType & type ) const
  {
    std::size_t idx( type == spo::asio::DataType::Input ? 0 : 1 );
    assert( idx < m_Servers.size() );
    return
        IsValid()
        ? m_Servers.at( idx )->SocketsValid()
        : -1;
  }

  /**
   * @brief SocketsCount
   * @return
   */
  int SocketsCount ( const spo::asio::DataType & type ) const
  {
    std::size_t idx( type == spo::asio::DataType::Input ? 0 : 1 );
    assert( idx < m_Servers.size() );
    return
        IsValid()
        ? m_Servers.at( idx )->SocketsCount()
        : -1;
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
    bool retval( IsValid() );
    if( retval )
    {
      for( auto & srv_ref : m_Servers )
      {
        if( not srv_ref->IsActive() )
        {
          std::make_shared< spo::thread_t >(
                boost::bind(
                  &server_t::Start,
                  srv_ref ) )->detach();
  //        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
          retval = retval and srv_ref->IsActive();
        }
      }
//      AsioService::Instance().Start();
    }
    return retval;
  }

  /**
   * @brief Метод Stop останавливает работу сервера по обслуживанию подключений/
   * @return Булево значение:
   * @value true сообщает об успешном останове сервера;
   * @value false сообщает о останове запуске.
   */
  bool Stop () BOOST_NOEXCEPT
  {
    bool retval( IsValid() );
    if( retval )
    {
      for( auto & srv_ref : m_Servers )
      {
        retval = retval and srv_ref->Stop();
      }
    }
    return retval;
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


#endif // ASIOSERVERDUPLEX_H
