#ifndef CLIENTSERVERBASE_H
#define CLIENTSERVERBASE_H

#include "asio/AsioService.h"
#include "asio/AsioSocketSession.h"
#include <mutex>

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
class SPO_CORE_EXPORT             ClientServerBase
{
public:
  using self_t                  = spo::asio::ClientServerBase< ProtocolT_, ByteT_ >;
  using service_t               = spo::asio::AsioService;
  using service_shr_t           = spo::asio::AsioServiceShared;
  using protocol_t              = ProtocolT_;

private:
  /**
   * @brief m_TransferType
   */
  std::atomic< spo::asio::TransferType >         m_TransferType  = spo::asio::TransferType::HalfDuplexIn;
  /**
   * @brief Атрибут m_Actions представляет ассоциативный контейнер соответствий
   *        индекса канала работы с данными функтору обработки данных.
   */
  buffer_actions_map< ByteT_ >    m_Actions
  {
    { 0, io_channel_action_t< ByteT_ >() },
    { 1, io_channel_action_t< ByteT_ >() },
  };
  /**
   * @brief Атрибут m_SocketsLimit содержит количество максимального допустимого
   *        значения одновременно обслуживаемых сокетов сервером.
   */
  std::atomic_int                 m_SocketsLimit      { ProtocolT_::socket::max_connections >> 1 };/* деление на 2 (для тех, кто на бронепоезде) */
  /**
   * @brief Атрибут m_SocketsCount содержит количество текущих обслуживаемых
   *        сокетов сервером.
   */
  std::atomic_int                 m_SocketsCount      { 0 };
  /**
   * @brief Атрибут m_SocketDeadline содержит значение задержки ожидания
   *        приема/передачи данных сокетом.
   */
  std::atomic< std::int64_t >     m_SocketDeadline    { 10000 };
  /**
   * @brief Атрибут m_TimeoutMs содержит значение задержки ожидания
   *        открытия/закрытия обработчика подключений.
   */
  std::atomic< std::int64_t >     m_TimeoutMs { 3000 };

  io_service_callback_t           m_SessionAfterStop;
  void                          * m_SessionAfterStopParamPtr = nullptr;

public:
  /**
   * @brief Конструктор ClientServerBase
   * @param type
   * @param serviceTimeoutMs
   */
  explicit ClientServerBase
  (
      const spo::asio::TransferType   type,
      const std::int64_t            & serviceTimeoutMs = 10000
  )
    : m_TransferType{ type }
  {
    UNUSED( spo::asio::AsioService::Instance( serviceTimeoutMs ) );
  }

  /**
   * @brief Метод TransferType значение типа обмена данными с сервером.
   * @return Значение типа обмена данными с сервером/
   */
  spo::asio::TransferType TransferType () const BOOST_NOEXCEPT
  {
    return m_TransferType.load();
  }

  /**
   * @brief Метод SetTransferType устанавливает значение типа (режима) обмена
   *        данными с сервером.
   * @param type значение типа обмена данными с сервером.
   */
  void SetTransferType ( const spo::asio::TransferType & type )
  {
    m_TransferType.store( type );
  }

  /**
   * @brief Метод SetTimeoutMs назначает величину ожидания попытки привязки
   *        приемника к сокету или ожидание подключения сокета клиента к серверу.
   * @param timeoutMs величина типа @a std::int64_t ожидания попытки привязки приемника к сокету.
   */
  void SetTimeoutMs ( const std::int64_t & timeoutMs )
  {
    m_TimeoutMs = timeoutMs > -1 ? timeoutMs : 0 ;
  }
  /**
   * @brief Метод TimeoutMs возвращает время ожидания подключения в миллисекундах
   * @return значение времени ожидания подключения в миллисекундах.
   */
  std::int64_t TimeoutMs () const
  {
    return m_TimeoutMs;
  }

  buffer_actions_map< ByteT_ >  & ActionsRef()
  {
    return std::ref( m_Actions );
  }

  /**
   * @brief Метод Action возвращает копию функтора обслуживания данных для
   *        запрашиваемого канала.
   * @param key признак канала типа @a spo::asio::DataType (ключ для std::map )
   * @return копия функтора типа @a io_channel_action_t обслуживания данных канала
   */
  spo::asio::io_channel_action_t< ByteT_ > & Action( const spo::asio::DataType & key )
  {
    return ActionsRef().at( key == spo::asio::DataType::Input ? 0 : 1 );
  }

  /**
   * @brief Метод SetBufferAction назначает действия при обмене данными с
   *        подключеными клиентами.
   * Методы назначаются для конкретного типа @a spo::asio::DataType канала данных.
   *
   * @param key     тип канала обмена данными:
   * @value spo::asio::DataType::Input канал приема данных от подключенного клиента;
   * @value spo::asio::DataType::Output канал передачи данных к клиенту сервера;
   * @param action  алгоритм обработки данных при приеме или передаче данных.
   *
   * Например:
   * @code
   * ...
   *   spo::asio::AsioServer< boost::asio::ip::tcp, char> server(
   *          spo::asio::TransferType::HalfDuplexIn,
   *          33444,
   *          boost::asio::ip::tcp::v4() );
   * ...
   *  server.SetBufferAction(
   *    spo::asio::DataType::Input,
   *    []( spo::core::net::DocumentPkg<char> & data )
   *    {
   *      bool retval( not data.IsEmpty() );
   *      if( retval )
   *      {
   *        std::cout << "\n\t-------------------- Receive --------------------" << std::endl;
   *        std::cout << data.ToByteArray().c_str() << std::endl;
   *        std::flush( std::cout );
   *      }
   *      return retval;
   *    } );
   *  server.SetBufferAction(
   *    spo::asio::DataType::Output,
   *    []( spo::core::net::DocumentPkg<char> & data )
   *    {
   *      auto tm = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
   *      std::string tstr( std::ctime(& tm ) );
   *      std::string str( 100000, 'S');
   *      str += "  " + tstr;
   *      str += "\n";
   *      data.FromByteArray( str );
   *      std::cout << "\n\t-------------------- Send --------------------" << std::endl;
   *      auto dumpstr( data.ToByteArray() );
   *      std::cout << std::string( dumpstr.c_str(), 80 ) <<  " ... " << std::string( dumpstr.c_str() + dumpstr.size() - 80, 80 ) << std::endl;
   *      std::flush( std::cout );
   *      return not data.IsEmpty();
   *    } );
   * ...
   * @endcode
   */
  void SetBufferAction
  (
      const spo::asio::DataType                         key,
      const spo::asio::io_channel_action_t< ByteT_ >  & action
  )
  {
    assert( key < spo::asio::DataType::DataSize );
    Action( key ) = action;
  }

  /**
   * @brief Метод SocketDeadline вщзвращает значение времени ожидания активации
   *        аксептора при запуске сервера.
   *
   * Если по истечении данного времени ( в миллисекундах) акцептор не готов к
   * работе, сервер выполняет попытку поереполключения или завершает работу.
   *
   * @return значение времени ожидания активации аксептора при запуске сервера.
   */
  boost::int64_t SocketDeadline ()
  {
    return m_SocketDeadline;
  }

  /**
   * @brief Метод SetSocketDeadline устанавливает значение времени ожидания активации
   *        аксептора при запуске сервера.
   *
   * Если по истечении данного времени ( в миллисекундах) акцептор не готов к
   * работе, сервер выполняет попытку поереполключения или завершает работу.
   *
   * @param socketDeadline значение времени ожидания активации
   *        аксептора при запуске сервера
   */
  void SetSocketDeadline ( const boost::int64_t & socketDeadline )
  {
    m_SocketDeadline.store(
        socketDeadline > 0 ?
          socketDeadline :
          0 );
  }

  /**
   * @brief Метод SocketsLimit возвращает максимальное допустимое значение
   *        количества одновременно обслуживаемых сокетов.
   * @return максимальное допустимое значение  количества одновременно
   *        обслуживаемых сокетов.
   */
  int SocketsLimit () const
  {
    return m_SocketsLimit;
  }

  /**
   * @brief Метод SetSocketsLimit устанавливает максимальное допустимое значение
   *        количества одновременно обслуживаемых сокетов.
   * @param socketsLimit значение максимального допустимого количества одновременно
   *        обслуживаемых сокетов
   */
  void SetSocketsLimit ( int socketsLimit )
  {
    m_SocketsLimit.store(
        socketsLimit >= ( ProtocolT_::socket::max_connections ) ?
          ( ProtocolT_::socket::max_connections -1 ) :
          socketsLimit );
  }

  /**
   * @brief Метод SocketsValid сообщает о допустимости работы с сокетами
   * @return Булево значение:
   * @value true  работа с сокетами возможна;
   * @value false работать с сокетами нельзя.
   */
  bool SocketsValid () const
  {
    return SocketsCount() < SocketsLimit();
  }

  /**
   * @brief Метод SocketsCount возвращает текущее значение открытых сокетов.
   * @return текущее значение типа @ int открытых сокетов.
   */
  int SocketsCount () const
  {
    return m_SocketsCount;
  }
  /**
   * @brief Метод SetSocketsCount устанавливает значение открытых на данный момент сокетов.
   * @param count значение открытых на данный момент сокетов.
   */
  void SetSocketsCount ( int count )
  {
    m_SocketsCount = count;
  }

  /**
   * @brief Метов IncSocketsCount увеличивает на 1 значение открытых на данный момент сокетов.
   */
  void IncSocketsCount ()
  {
    m_SocketsCount++;
  }

  /**
   * @brief Метов IncSocketsCount увеличивает на 1 значение открытых на данный момент сокетов.
   */
  void DecSocketsCount ()
  {
    m_SocketsCount =
        m_SocketsCount > 0
        ? m_SocketsCount - 1
        : 0;
  }

  /**
   * @brief Метод IsFullDuplex сообщает признак того, что сервер рваботает в
   *        режиме полнлго дуплекса ассинхронной передачи данных по независимым
   *        каналам с различными значеничми портов.
   * @return Булево значение:
   * @value true  сервер работает в режиме полного дуплекса;
   * @value false сервер не работает в режиме полного дуплекса.
   */
  bool IsFullDuplex () const BOOST_NOEXCEPT
  {
    return TransferType() == spo::asio::TransferType::FullDuplex;
  }

  /**
   * @brief Метод IsHalfDuplexIn сообщает признак того, что сервер рваботает в
   *        режиме полудуплекса последовательной передачи данных с первоначальным
   *        приемом сервером информации от клиента.
   * @return Булево значение:
   * @value true  сервер работает в режиме полудуплекса;
   * @value false сервер не работает в режиме полудуплекса.
   */
  bool IsHalfDuplexIn () const BOOST_NOEXCEPT
  {
    return TransferType() == spo::asio::TransferType::HalfDuplexIn;
  }

  /**
   * @brief IsHalfDuplexOut сообщает признак того, что сервер рваботает в
   *        режиме полудуплекса последовательной передачи данных с первоначальной
   *        отправкой сервером информации от клиента.
   * @return Булево значение:
   * @value true  сервер работает в режиме полудуплекса;
   * @value false сервер не работает в режиме полудуплекса.
   */
  bool IsHalfDuplexOut () const BOOST_NOEXCEPT
  {
    return TransferType() == spo::asio::TransferType::HalfDuplexOut;
  }

  bool IsHalfDuplex () const BOOST_NOEXCEPT
  {
    return IsHalfDuplexIn() or IsHalfDuplexOut();
  }

  /**
   * @brief IsSimplexIn сообщает признак того, что сервер рваботает в
   *        режиме симплекса по приему данных от клиента.
   * @return Булево значение:
   * @value true  сервер работает в режиме симплекса по приему данных;
   * @value false сервер не работает в режиме симплекса по приему данных.
   */
  bool IsSimplexIn () const BOOST_NOEXCEPT
  {
    return TransferType() == spo::asio::TransferType::SimplexIn;
  }

  /**
   * @brief IsSimplexOut сообщает признак того, что сервер рваботает в
   *        режиме симплекса по передаче данных к клиенту.
   * @return Булево значение:
   * @value true  сервер работает в режиме симплекса передаче данных;
   * @value false сервер не работает в режиме симплекса по передаче данных.
   */
  bool IsSimplexOut () const BOOST_NOEXCEPT
  {
    return TransferType() == spo::asio::TransferType::SimplexOut;
  }

  /**
   * @brief Метод IsValid готовность к приему подключения
   * @return Булево значение:
   * @value true  сервер готов к обслуживанию клиентов;
   * @value false сервер не готов к обслуживанию клиентов.
   */
  virtual bool IsValid () const
  {
    return spo::asio::AsioService::Instance().IsStateValid();
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
    return self_t::service_t::Instance().IsActive();
  }

  /**
   * @brief SetSessionAfterStop
   * @param fnc
   * @param paramPtr
   */
  void SetSessionAfterStop  ( const io_service_callback_t & fnc, void * paramPtr = nullptr )
  {
    m_SessionAfterStop = fnc;
    m_SessionAfterStopParamPtr = paramPtr;
  }

  /**
   * @brief ClearSessionAfterStop
   */
  void ClearSessionAfterStop  ()
  {
    SetSessionAfterStop( io_service_callback_t(), nullptr );
  }

  /**
   * @brief SessionAfterStop
   * @return
   */
  io_service_callback_t SessionAfterStop  ()
  {
    return m_SessionAfterStop;
  }

  /**
   * @brief SessionAfterStopParam
   * @return
   */
  void * SessionAfterStopParam ()
  {
    return m_SessionAfterStopParamPtr;
  }
};

//------------------------------------------------------------------------------

}// namespace                     asio
}// namespace                     spo


#endif // CLIENTSERVERBASE_H
