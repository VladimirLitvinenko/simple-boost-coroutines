#ifndef ASIOSOCKETSESSION_H
#define ASIOSOCKETSESSION_H

#include "asio/AsioService.h"
#include "asio/AsioError.h"
#include "asio/IOChannel.h"

namespace                         spo   {
namespace                         asio  {

//------------------------------------------------------------------------------

/**
 * @brief Тип io_buffers_t
 */
template
<
    typename                      ProtocolT_,
    typename                      ByteT_
>
using io_buffers_t              = std::vector< IOChannel< ByteT_ > >;

template
<
    typename Prot_,
    typename                    = typename std::enable_if
    <
      std::is_same< Prot_, boost::asio::ip::tcp >::value
      or
      std::is_same< Prot_, boost::asio::ip::udp >::value
//      or
//      std::is_same< Prot_, boost::asio::ip::icmp >::value
    >::type
>
bool SetSocketOptions( typename Prot_::socket & )
{
  return false;
}

template<>
bool SetSocketOptions< boost::asio::ip::tcp >
( boost::asio::ip::tcp::socket & socket )
{
  bool retval( socket.is_open() );
  if( retval )
  {
    socket.set_option( boost::asio::ip::tcp::no_delay(true) );
    socket.non_blocking( true );

    boost::asio::socket_base::enable_connection_aborted r_aborted( true );
    socket.set_option( r_aborted ) ;
    boost::asio::socket_base::reuse_address r_option(true);
    socket.set_option( r_option ) ;
  }
  return retval;
}

template<>
bool SetSocketOptions< boost::asio::ip::udp >( boost::asio::ip::udp::socket & socket )
{
  bool retval( socket.is_open() );
  if( retval )
  {
    socket.non_blocking( true );

    boost::asio::ip::udp::socket::broadcast bcast( true );
    socket.set_option( bcast );
    boost::asio::socket_base::enable_connection_aborted r_aborted( true );
    socket.set_option( r_aborted ) ;
    boost::asio::socket_base::reuse_address r_option(true);
    socket.set_option( r_option ) ;
  }
  return retval;
}


//------------------------------------------------------------------------------
/**
 * @brief Шаблонная структура async_reader определяет специализацию
 *        опрератора operator() для получения данных из сокета определенного
 *        типа протокола.
 * @param Prot_ тип протокола
 */
template
<
    typename SocketSession,
    typename Prot_,
    typename                    = typename std::enable_if
    <
      std::is_same< Prot_, boost::asio::ip::tcp >::value
      or
      std::is_same< Prot_, boost::asio::ip::udp >::value
//      or
//      std::is_same< Prot_, boost::asio::ip::icmp >::value
    >::type
>
struct async_reader
{
    std::size_t operator()
    (
        SocketSession &,
        boost::asio::streambuf::mutable_buffers_type &,
        boost::system::error_code &,
        boost::asio::yield_context
    ) const
    {
      assert( false );
      return 0;
    }
};

/**
 * @brief Шаблонная структура async_reader определяет реализацию
 *        опрератора operator() для получения данных из сокета TCP-протокола.
 */
template < typename SocketSession >
struct async_reader< SocketSession, boost::asio::ip::tcp >
{
  std::size_t operator()
  (
      SocketSession                               & session,
      boost::asio::streambuf::mutable_buffers_type& bufs,
      boost::system::error_code                   & ec,
      boost::asio::yield_context                    yield
  ) const
  {
    return session.SocketRef().async_read_some( bufs, yield[ ec ] );
  }
};

/**
 * @brief Шаблонная структура async_reader определяет реализацию
 *        опрератора operator() для получения данных из сокета UDP-протокола.
 */
template < typename SocketSession >
struct async_reader< SocketSession, boost::asio::ip::udp >
{
  std::size_t operator()
  (
      SocketSession                               & session,
      boost::asio::streambuf::mutable_buffers_type& bufs,
      boost::system::error_code                   & ec,
      boost::asio::yield_context                    yield
  ) const
  {
    return session.SocketRef().async_receive_from( bufs, session.EndpointRef(), yield[ ec ] );
  }
};

//------------------------------------------------------------------------------
/**
 * @brief Шаблонная структура async_writer определяет специализацию
 *        опрератора operator() для отправки данных в сокет определенного
 *        типа протокола.
 * @param Prot_ тип протокола
 */
template
<
    typename SocketSession,
    typename Prot_,
    typename                    = typename std::enable_if
    <
      std::is_same< Prot_, boost::asio::ip::tcp >::value
      or
      std::is_same< Prot_, boost::asio::ip::udp >::value
//      or
//      std::is_same< Prot_, boost::asio::ip::icmp >::value
    >::type
>
struct async_writer
{
    std::size_t operator()
    (
        SocketSession &,
        boost::asio::streambuf &,
        boost::system::error_code &,
        boost::asio::yield_context
    ) const
    {
      assert( false );
      return 0;
    }
};

/**
 * @brief Шаблонная структура async_writer определяет реализацию
 *        опрератора operator() для отправки данных в сокет TCP-протокола.
 */
template < typename SocketSession >
struct async_writer< SocketSession, boost::asio::ip::tcp >
{
  std::size_t  operator()
  (
      SocketSession             & session,
      boost::asio::streambuf    & bufs,
      boost::system::error_code & ec,
      boost::asio::yield_context  yield
  ) const
  {
    return session.SocketRef().async_write_some( bufs.data(), yield[ ec ] );
  }
};

/**
 * @brief Шаблонная структура async_writer определяет реализацию
 *        опрератора operator() для отправки данных в сокет UDP-протокола.
 */
template < typename SocketSession >
struct async_writer< SocketSession, boost::asio::ip::udp >
{
  virtual std::size_t  operator()
  (
      SocketSession & session,
      boost::asio::streambuf & bufs,
      boost::system::error_code & ec,
      boost::asio::yield_context yield
  ) const
  {
    return session.SocketRef().async_send_to( boost::asio::buffer( bufs.data() ), session.EndpointRef(), yield[ ec ] );
  }
};

//------------------------------------------------------------------------------

/**
 * @brief Шаблонный класс AsioSocketSession определяет атрибуты и методы работы
 *        с IP-сокетом передачи сетевых данных.
 *
 * Параметры шаблона:
 * @value ProtocolT_ тип протокола для обмена данными по сети.
 * @value ByteT_ тип единицы информации для каналов обмена данными.
 *
 * Объект сессии запускается как std::shared_ptr для корректного завершения работы
 * с сокетом при отключении от нее ведущих обьектов приема / передачи данных таких
 * как @a boost::asio::ip::tcp::acceptor и @a boost::asio::connect
 *
 * @par Пример использования:
 * @code language="cpp"
 * . . .
 *  m_Acceptor.async_accept( socket, yield[ ec ] );
 *
 *  if( IsNoErr( ec ) )
 *  {
 *    auto session_ptr( std::make_shared< AsioSocketSession<boost::asio::ip::tcp, char> >(
 *                        TransferType::SimplexIn,
 *                        std::move( socket ),
 *                        1000 ) );
 *    if( session_ptr )
 *    {
 *      . . .
 *      session_ptr->Start();
 *    }
 *    . . .
 *  }
 *
 * @endcode
 */
template
<
    typename                      ProtocolT_,
    typename                      ByteT_ = unsigned char,
    typename                    = typename std::enable_if
    <
      std::is_same< ProtocolT_, boost::asio::ip::tcp >::value
      or
      std::is_same< ProtocolT_, boost::asio::ip::udp >::value
      or
      std::is_same< ProtocolT_, boost::asio::ip::icmp >::value
    >::type
>
class SPO_CORE_EXPORT             AsioSocketSession :
public                            std::enable_shared_from_this< spo::asio::AsioSocketSession< ProtocolT_, ByteT_ > >
{
public:
  using socket_t                = typename ProtocolT_::socket;
  using endpoint_t              = typename ProtocolT_::endpoint;
  using socketptr_t             = std::shared_ptr< socket_t >;
  using self_t                  = spo::asio::AsioSocketSession< ProtocolT_, ByteT_ >;
  using shared_t                = std::enable_shared_from_this< self_t >;
  using buffer_container_t      = io_buffers_t< ProtocolT_, ByteT_ >;
  using timer_ptr               = std::shared_ptr< SteadyTimer >;

private:
  /**
   * @brief Атрибут m_Socket содержит прикрепленный к сесии сокет канала приема
   * и отправки сетевых данных.
   */
  socket_t                        m_Socket;
  /**
   * @brief m_Endpoint
   */
  endpoint_t                      m_Endpoint;
  /**
   * @brief Атрибут m_TransferType содержит характеристики сессии по типу передачи
   * данных: симплексный прием, симплексная передача и т.д.
   *
   * @see spo::asio::TransferType
   */
  std::atomic< spo::asio::TransferType >  m_TransferType { spo::asio::TransferType::SimplexIn };
  /**
   * @brief Атрибут m_Channels содержит набор каналов для приема и передачи данных
   *        через сокет @a m_Socket.
   *
   * Атрибут m_Channels определен как контейнер типа  @a std::vector экземпляров
   * каналов для передачи данных.
   *
   * @see spo::asio::io_buffers_t, spo::asio::DataType, spo::asio::IOChannel
   */
  buffer_container_t              m_Channels
  {
    io_channel_action_t< ByteT_ >(),
    io_channel_action_t< ByteT_ >(),
  };

  /**
   * @brief Атрибут m_Error содержит код выполнения метода библиотеки Boost.Asio.
   * @see spo::asio::AsioError
   */
  AsioError                       m_Error;
  /**
   * @brief Атрибут m_TimerPtr содержит время ожидания обработки данных буфера обмена.
   * Значение "завернуто" в "уный" указатель общего доступа @a std::shared_ptr
   */
  timer_ptr                       m_TimerPtr;
  std::atomic<std::size_t>        m_Transfered    { 0 };
  spo::simple_fnc_t<void>         m_AfterTransfer;

  /**
   * @brief m_AfterStop
   */
  io_service_callback_t           m_AfterStop;
  void                          * m_StopParamPtr = nullptr;

  void SetTransfered( const std::size_t value, bool onTransferedExec = false )
  {
    m_Transfered.store( value );
    if( onTransferedExec and IsTransfered() and m_AfterTransfer.operator bool() )
    {
      std::async( std::launch::async, boost::bind( &self_t::m_AfterTransfer, this ) ).
          wait_for( std::chrono::milliseconds(0));
    }
  }

public:
  /**
   * @brief Конструктор AsioSocketSession принимает ссылку на сервис boost::asio::io_service
   *        и определяет режимы и время ожидания ответа приема и передачи данных.
   * @param type тип передачи данных
   * @param socket r-value ссылка на сокет
   * @param deadLine время ожидания завершкения приема/передачи данныхчерез сетевой
   *        сокет @a m_Socket;
   *
   * Конструктор закрепляет за сессией владение сокетом ( @a std::move )
   *
   * @see spo::asio::TransferType
   */
  /* конструктор */               AsioSocketSession
  (
      const spo::asio::TransferType & type,
      typename ProtocolT_::socket  && socket,
      const boost::int64_t            deadLine  = boost::int64_t( ASIO_DEADLINE_DEFAULT * 10 )
  )
    : m_Socket    ( std::move( socket ) )
    , m_TimerPtr  ( std::make_shared< timer_ptr::element_type>(
                                          m_Socket.get_io_service(),
                                          deadLine ) )
  {
    assert( m_TimerPtr );
    SetTransferType( type );
    SetDefaultErrorCallbacks();
  }

  /* конструктор */               AsioSocketSession
  (
      const spo::asio::TransferType & type,
      typename ProtocolT_::socket  && socket,
      typename ProtocolT_::endpoint   ep,
      boost::int64_t                  deadLine  = ASIO_DEADLINE_DEFAULT * 10
  )
    : AsioSocketSession( type, std::move( socket ), deadLine )
  {
    m_Endpoint = ep;
  }

  virtual ~AsioSocketSession()
  {
    Stop();
  }

  endpoint_t & EndpointRef()
  {
    return std::ref( m_Endpoint );
  }

  /**
   * @brief Метод TransferType возвращает тип (режим) работы сессии:
   * @return значение типа работы сессии типа @a spo::asio::TransferType :
   * @value SimplexIn     одностороний прием данных (без ожидания и обработки ответа)
   * @value SimplexOut    односторонняя передача данных (без ожидания и обработки према данных )
   * @value HalfDuplexIn  двусторонний последовательный обмен данными ( прием затем передача )
   * @value HalfDuplexOut двусторонний последовательный обмен данными ( передача затем приём )
   *
   * @warning FullDuplex НЕ ИСПОЛЬЗУЕТСЯ ЯВНО!
   */
  spo::asio::TransferType TransferType () const BOOST_NOEXCEPT
  {
    return m_TransferType;
  }

  /**
   * @brief Метод SetTransferType назначает тип (режим) работы текущей сессии
   * @param type значение типа работы ntreotq сессии:
   * @value SimplexIn     одностороний прием данных (без ожидания и обработки ответа)
   * @value SimplexOut    односторонняя передача данных (без ожидания и обработки према данных )
   * @value HalfDuplexIn  двусторонний последовательный обмен данными ( прием затем передача )
   * @value HalfDuplexOut двусторонний последовательный обмен данными ( передача затем приём )
   *
   * @warning FuulDuplex НЕ ИСПОЛЬЗУЕТСЯ ЯВНО: будет задано исключение.
   */
  void SetTransferType ( const spo::asio::TransferType & type ) BOOST_NOEXCEPT
  {
    // полный дуплекс реализуется двумя сессиями для двух однонаправленных сокетов!
    assert( type !=  spo::asio::TransferType::FullDuplex );
    m_TransferType.store( type );
  }

  /**
   * @brief Метод StopTimer останавливает таймер @a m_TimerPtr задержки приема
   *        или передачи данных через сокет.
   */
  void                            StopTimer   ()
    { if( m_TimerPtr ) m_TimerPtr->Stop(); }

  /**
   * @brief Метод StartTimer pfgrcrftn таймер @a m_TimerPtr задержки приема
   *        или передачи данных через сокет.
   * По истечению времени задержки прием или передача прекращаются.
   */
  void                            StartTimer   ()
    { if( m_TimerPtr ) m_TimerPtr->Start(); }

  /**
   * @brief Метод TimerRef возвращает ссылку на общий указатель, содержащий
   *        переменную со значением времени ожидания приема или передачи данных.
   * @return ссылку на общий указатель.
   */
  timer_ptr                     & TimerRef      ()
    { return std::ref( m_TimerPtr ); }

  /**
   * @brief Метод IsTimerActive возвращает пнризнак раброты таймера (запущен
   * ли таймер ожидания приема или передпачи информации).
   * @return Признак:
   * @value true  таймер запущен;
   * @value false таймер остановлен (не запущен)
   */
  bool                            IsTimerActive () const
    { return m_TimerPtr ? m_TimerPtr->IsActive() : false; }


  /**
   * @brief Метод IsHalfDuplexIn сообщает, применяется ли сессией значение
   * @a spo::asio::TransferType::HalfDuplexIn типа @a spo::asio::TransferType
   * @return булево значение:
   * @value true  сессия находится в режиме двустороннего последовательного
   *              обмена данными ( прием затем передача )
   * @value false сессия не находится в вышеупомянутом режиме
   */
  bool IsHalfDuplexIn () const BOOST_NOEXCEPT
  {
    return TransferType() == spo::asio::TransferType::HalfDuplexIn;
  }

  /**
   * @brief Метод IsHalfDuplexOut сообщает, применяется ли сессией значение
   * @a spo::asio::TransferType::HalfDuplexOut типа @a spo::asio::TransferType
   * @return булево значение:
   * @value true  сессия находится в режиме двустороннего последовательного
   *              обмена данными ( передача затем приём )
   * @value false сессия не находится в вышеупомянутом режиме
   */
  bool IsHalfDuplexOut () const BOOST_NOEXCEPT
  {
    return TransferType() == spo::asio::TransferType::HalfDuplexOut;
  }

  /**
   * @brief IsHalfDuplex сообщает, находится ли сессия хотя бы в в режиме
   *        двустороннего последовательного обмена данными.
   * @return булево значение:
   * @value true  сессия находится в режиме двустороннего последовательного
   *              обмена данными
   * @value false сессия не находится в вышеупомянутом режиме
   */
  bool IsHalfDuplex () const BOOST_NOEXCEPT
  {
    return IsHalfDuplexIn() or IsHalfDuplexOut();
  }

  /**
   * @brief Метод IsSimplexIn сообщает, применяется ли сессией значение
   * @a spo::asio::TransferType::SimplexIn типа @a spo::asio::TransferType
   * @return булево значение:
   * @value true  сессия находится в режиме односторонего приема данных (без
   *              ожидания и обработки ответа)
   * @value false сессия не находится в вышеупомянутом режиме
   */
  bool IsSimplexIn () const BOOST_NOEXCEPT
  {
    return TransferType() == spo::asio::TransferType::SimplexIn;
  }

  /**
   * @brief Метод IsSimplexOut сообщает, применяется ли сессией значение
   * @a spo::asio::TransferType::SimplexOut типа @a spo::asio::TransferType
   * @return булево значение:
   * @value true  сессия находится в режиме одностороней передачи данных (без
   *              ожидания и обработки приема)
   * @value false сессия не находится в вышеупомянутом режиме
   */
  bool IsSimplexOut () const BOOST_NOEXCEPT
  {
    return TransferType() == spo::asio::TransferType::SimplexOut;
  }

  /**
   * @brief Метод IsSimplex  сообщает, находится ли сессия хотя бы в в режиме
   *        одностороннего обмена данными.
   * @return булево значение:
   * @value true  сессия находится в режиме одностороннего обмена данными
   * @value false сессия не находится в в режиме одностороннего обмена данными
   */
  bool IsSimplex () const BOOST_NOEXCEPT
  {
    return IsSimplexIn() or IsSimplexOut();
  }

  /**
   * @brief Метод IsOpen возвращает признак открытого сокета.
   * @return булево значение:
   * @value true  сокет открыт;
   * @value false сокет закрыт.
   */
  bool IsOpen () const BOOST_NOEXCEPT
  {
    return m_Socket.is_open();
  }

  /**
   * @brief Метод ServiceRef возвращает ссылку на сервис воода/вывода
   *        типа @a boost::asio::io_service для сокета @a m_Socket.
   * @return ссылку на сервис воода/вывода
   */
  io_service_t & ServiceRef ()
  {
    return m_Socket.get_io_service();
  }

  /**
   * @brief Метод ChannelsRef
   * @return
   */
  buffer_container_t & ChannelsRef ()
  {
    return std::ref( m_Channels );
  }

  /**
   * @brief Transfered
   * @return
   */
  std::size_t Transfered() const
  {
    return m_Transfered.load();
  }

  /**
   * @brief IsTransfered
   * @return
   */
  bool IsTransfered () const
  {
    return Transfered() > 0;
  }

  /**
   * @brief SetAfterTransfer
   * @param f
   */
  void SetAfterTransfer( const spo::simple_fnc_t<void> & f )
  {
    m_AfterTransfer = f;
  }

  /**
   * @brief Метод Start запускает обработку приема и отправки данных через сокет.
   *
   * Метод реализует методы чередования приема и передачи как набор сопрограмм.
   * Формируется общий умный указатель @a std::shared_ptr на текущий (this)
   * экземпляр класса. Необходим для корректного завершения сопрограм без потери
   * указателя на текущий экземпляр.
   *
   * @see boost::asio::spawn, boost::bind, std::enable_shared_from_this
   * @see Boost.Coroutine
   */
  void Start ()
  {
    if( not SetSocketOptions< ProtocolT_ >( SocketRef() ) )
    {
      Stop();
      return;
    }

    auto self( this->shared_from_this() );

    try
    {
      switch( self->TransferType() )
      {
        case spo::asio::TransferType::SimplexIn :
        { // прем данных выполняется первым.
          boost::asio::spawn(
                io_strand_t( self->ServiceRef() ),
                boost::bind( & self_t::Receive, self, _1  ) );
        }
        break;

        case spo::asio::TransferType::SimplexOut :
        { // передача данных выполняется первой
          boost::asio::spawn(
                io_strand_t( self->ServiceRef() ),
                boost::bind( & self_t::Send, self, _1 ) );
        }
        break;

        case spo::asio::TransferType::HalfDuplexIn :
        { // прем данных выполняется первым, затем идет передача
          boost::asio::spawn(
                io_strand_t( self->ServiceRef() ),
                [ this, self ]( boost::asio::yield_context yield )
                {
                  spo::asio::error_t  ec;

                  self->Receive ( yield[ ec ] );
                  if( ( not spo::asio::AsioService::Instance().IsError( ec ) ) and self->IsTransfered() )
                  {
                    self->Send( yield[ec] );
                  }
                  if( spo::asio::AsioService::Instance().IsError( ec ) )
                  {
                    self->Stop();
                  }
                } );
        }
        break;

        case spo::asio::TransferType::HalfDuplexOut :
        { // передача данных клиенту выполняется первой, затем следует прием
          boost::asio::spawn(
                io_strand_t( self->ServiceRef() ),
                [ this, self ]( boost::asio::yield_context yield )
                {
                  spo::asio::error_t  ec;

                  self->Send ( yield[ ec ] );

                  if( not spo::asio::AsioService::Instance().IsError( ec ) and self->IsTransfered() )
                  {
                    self->Receive ( yield );
                  }
                  if( spo::asio::AsioService::Instance().IsError( ec ) )
                  {
                    self->Stop();
                  }
                } );
        }
        break;
        default : throw boost::system::errc::invalid_argument;
      }

      // запуск алгоритма, выполняемого по окончании таймаута ожидания передачи
      // данных по сокету.
      boost::asio::spawn(
            io_strand_t( self->ServiceRef() ),
            boost::bind( & self_t::CheckTimeout, self, _1 ) );
    }
    catch( const std::exception & e )
    {
      UNUSED( AsioService::ExceptionError() );
      DUMP_EXCEPTION( e );
    }
  }

  /**
   * @brief Метод Stop выполняет останов сессии работы с сокетом.
   *
   * Таймеры всех каналов приема и отправки данных прекращают отсчет ожидания,
   * буферы с данными очищаются, сокет закрывается.
   */
  void Stop ()
  {
    spo::asio::error_t ec;
    try
    {
      StopTimer();
      if( IsOpen() )
      {
        m_Socket.shutdown( boost::asio::socket_base::shutdown_both, ec );
      }
    }
    catch( const std::exception & e )
    {
      DUMP_EXCEPTION( e );
    }

    m_Socket.close( ec );
    if( m_AfterStop )
    {
//      std::async( std::launch::async, &self_t::m_AfterStop, this, m_StopParamPtr ).
//          wait_for( std::chrono::milliseconds(0));
      std::async( std::launch::async, [this](){ this->m_AfterStop( m_StopParamPtr ); } ).
          wait_for( std::chrono::milliseconds(0));
    }
  }

  /**
   * @brief Метод SocketRef возвращает ссылку на сокет.
   * @return Ссылка на сокет.
   */
  socket_t & SocketRef ()
  {
    return std::ref( m_Socket );
  }

  /**
   * @brief Метод SetAction устанавливает действие для обработки/подготовки данных сокета.
   * @param idx индекс канала данных
   * @param action действие по обработке/подготовке данных
   */
  void SetAction ( std::size_t idx, const io_channel_action_t< ByteT_ > & action )
  {
    m_Channels[ idx ].SetAction( action );
  }

  /**
   * @brief Метод SetActionTx устанавливает действие для формирования набора данных
   *        перед отправкой через сокет @a m_Socket.
   * @param action ссылка на функтор с действием по обработке данных
   */
  void SetActionTx ( const io_channel_action_t< ByteT_ > & action )
  {
    m_Channels[ 1 ].SetAction( action );
  }

  /**
   * @brief Метод SetError
   * @param error
   */
  virtual void SetError ( const AsioError & error )
  {
    m_Error.SetErrorCode( error.Code() );
  }

  /**
   * @brief Метод SetError
   * @param error
   */
  virtual void SetError ( AsioError && error )
  {
    m_Error.SetErrorCode( std::move( error ).Code() );
  }

  /**
   * @brief Метод SetError
   * @param error
   */
  virtual void SetError ( const error_t & error )
  {
    m_Error.SetErrorCode( error );
  }

  /**
   * @brief Метод SetError
   * @param ec
   */
  virtual void SetError ( boost::system::errc::errc_t ec )
  {
    m_Error.SetErrorCode( ec );
  }

  /**
   * @brief Метод ErrorCode
   * @return
   */
  error_t ErrorCode () const
  {
    return m_Error.Code();
  }

  /**
   * @brief Метод ErrorValue
   * @return
   */
  boost::system::errc::errc_t ErrorValue () const
  {
    return m_Error.Value();
  }

  /**
   * @brief Метод SetErrorCallbacks  устанавливает обработчики для ошибок Boost.Asio.
   * @param callbacks ассоциатикный контейнер обработчиков для ошибок Boost.Asio:
   * @value asio_errcallbacks_t::key_type значение ошибки Boost.Asio;
   * @value asio_errcallbacks_t::mapped_type обработчик ошибки
   */
  void SetErrorCallbacks ( const asio_errcallbacks_t& callbacks ) BOOST_NOEXCEPT
  {
    m_Error.SetErrorCallbacks( callbacks );
  }

  /**
   * @brief Метод SetErrorCallback устанавливает обработчик для определенной
   *        ошибки Boost.Asio.
   * @param err     значение ошибки Boost.Asio;
   * @param action  обработчик ошибки @a err.
   */
  void SetErrorCallback
  (
      const asio_errcallbacks_t::key_type & err,
      const asio_errcallbacks_t::mapped_type & action
  )
  BOOST_NOEXCEPT
  {
    m_Error.SetErrorCallback( err, action );
  }

  /**
   * @brief Метод SetDefaultErrorCallbacks назначает обработчик по-умолчанию
   *        ошибок Boost.Asio
   */
  void SetDefaultErrorCallbacks () BOOST_NOEXCEPT
  {
    SetErrorCallbacks
        ( {
            { // обработка пользовательского исключения
              boost::system::errc::errc_t::owner_dead,
              boost::bind( &self_t::Stop, this )
            },
          } );
  }

  /**
   * @brief Метод Receive реализует сопрограмму по приему данных из сокета.
   * @param yield контент условия передачи управления сопрограме.
   */
  void Receive( boost::asio::yield_context yield )
  {
    error_t ec;
    try
    {
      auto & ch_ref = ChannelsRef().at(0);

      if( IsOpen() and ( SocketRef().available( ec ) > 0)/* and IsNoErr( ec )*/ )
      {
        // сокет располагает данными для приема в буфер
        // подготовка размера промежуточного буфера
        boost::asio::streambuf buffer;
        boost::asio::streambuf::mutable_buffers_type bufs(
              buffer.prepare( ch_ref.BufferSize() ) );

        // запуск таймера ожидания приема данных
        StartTimer();

        // асинхронный прием данных с получением значения фактически принятых данных
        SetTransfered(
            async_reader< AsioSocketSession< ProtocolT_, ByteT_ >, ProtocolT_ >()(
              *this, bufs, ec, yield ) );

        auto t( Transfered() );
        buffer.commit( t );
        if( t > 0 )
        {
          // таймер остановлен, т.к. данные получены
          StopTimer();

          if( ch_ref.ActionExists() )
          { // обработчик данных присутствует
            ec = boost::system::errc::make_error_code( boost::system::errc::success );

            // данные приняты и действие над данными в буфере опаределено
            // перенос данных из временного буфера в m_Buffer
            ch_ref.BufferRef().FromStream( buffer );

            // выполнение действия над данными буфера
            ch_ref.Execute();

            // очистка данных (данные больше не нужны, т.к. ими управляет
            // обработчик @a m_Action)
            ch_ref.Clear();
          }
        }
        SetTransfered( t, true );
      }
    }
    catch (std::exception& e)
    {
      ec = AsioService::ExceptionError();
      DUMP_EXCEPTION( e );
    }
  }

  /**
   * @brief Метод Send реализует сопрограмму по отправке данных через сокет.
   * @param yield контент условия передачи управления сопрограме.
   */
  void Send( boost::asio::yield_context yield )
  {
    error_t ec;
    try
    {
      auto & ch_ref = ChannelsRef().at( 1 );
      if( IsOpen() and ch_ref.ActionExists() )
      {
        // очиска буфера
        ch_ref.Clear();

        // подготовка буфера с данными
        ch_ref.Execute();

        if( not ch_ref.BufferRef().IsEmpty() )
        { // буфер содержит данных к отправке
          // перенос данных во временный буфер к оправке на сокет
          boost::asio::streambuf buffer;
          ch_ref.BufferRef().ToStream( buffer, ch_ref.BufferRef().Size() );

          // запуск таймера ожидания передачи данных
          StartTimer();

          // попытка передачи данных в сокет
          SetTransfered(
              async_writer< AsioSocketSession<ProtocolT_,ByteT_>, ProtocolT_ >()(
                *this, buffer, ec, yield ), true );

          if( not spo::asio::AsioService::Instance().IsError( ec ) )
          { // передача выполнена: останов таймера
            StopTimer();
          }
        }
      }
    }
    catch( const std::exception & e )
    {
      ec = AsioService::ExceptionError();
      DUMP_EXCEPTION( e );
    }
  }

  /**
   * @brief Метод CheckTimeout возвращает признак продолжения работы сопрограммы
   * по таймауту.
   * @param socket ссылка на сокет, к которому "привязан" таймер
   * @param yield   метод условия передачи управления сопрограмме или продолжения
   *                выполнения текущего метода.
   * @return Признак продолжения работы:
   * @value true работа (чтение или запись через сокет ) может быть продолжена
   * @value false работа должна быть прервана по таймауту или по закрытию сокета.
   */
  void CheckTimeout( boost::asio::yield_context yield )
  {
    auto self( this/*->shared_from_this()*/ );
    boost::system::error_code ignored_ec;
    try
    {
      if( SocketRef().is_open() )
      {
        if( self->IsTimerActive() )
        {
          // асинжронно выполнить ожидание в течение заданного времени
          self->TimerRef()->TimerRef().async_wait( yield[ ignored_ec ] );
          if( self->TimerRef()->IsExpired( ignored_ec ) )
          { // время ожидания приема/передачи через сокет истекло
            self->Stop();
            yield[ ignored_ec ];
          }
        }
      }
    }
    catch( const std::exception & e )
    {
      ignored_ec = AsioService::ExceptionError();
      DUMP_EXCEPTION( e );
    }
  }

  /**
   * @brief SetAfterStop
   * @param f
   * @param stopParamPtr
   */
  void SetAfterStop( const spo::asio::io_service_callback_t & f, void * stopParamPtr = nullptr )
  {
    m_AfterStop = f;
    m_StopParamPtr = stopParamPtr;
  }
};

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
using SocketSessionShared = std::shared_ptr< spo::asio::AsioSocketSession< ProtocolT_, ByteT_ > >;


template
<
    typename                      ProtocolT_,
    typename                      ByteT_,
    typename                  ... Args,
    typename                    = typename std::enable_if
    <
      std::is_same< ProtocolT_, boost::asio::ip::tcp >::value
      or
      std::is_same< ProtocolT_, boost::asio::ip::udp >::value
      or
      std::is_same< ProtocolT_, boost::asio::ip::icmp >::value
    >::type
>
SocketSessionShared< ProtocolT_, ByteT_ > MakeSocketSession
(
    buffer_actions_map< ByteT_ >  & actions,
    Args                        ... params
)
{
  assert( actions.size() == spo::asio::DataType::DataSize );

  SocketSessionShared< ProtocolT_, ByteT_ > session_ptr(
      std::make_shared<
        AsioSocketSession< ProtocolT_, ByteT_ > >(
          std::forward< Args >( params ) ... ) );

  if( session_ptr )
  {
    if( not actions.empty() )
    {
      for( auto act_ref : actions )
      { // назначение методов обработки данных для каналов проиема/передачи
        session_ptr->SetAction( static_cast<std::size_t>( act_ref.first ),
                                act_ref.second );
      }
    }
  }
  else
  {
    spo::asio::AsioService::Instance().SetError( boost::system::errc::errc_t::owner_dead );
  }

  return std::move( session_ptr );
}

//------------------------------------------------------------------------------

}// namespace                   asio
}// namespace                   spo

#endif // ASIOSOCKETSESSION_H
