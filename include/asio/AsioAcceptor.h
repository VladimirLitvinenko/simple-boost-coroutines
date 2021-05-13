/**
 * @file   AsioAcceptor.h
 * @brief  Файл содержит описание и реализацию шаблонного класса AsioAcceptor
 *         обслуживания подключений к серверу по IP-протоколу для обмена данных
 *         в сети.
 *
 * @author Vladimir N. Litvinenko
 *
 */
#ifndef ASIO_ACCEPTOR_H
#define ASIO_ACCEPTOR_H

#include "asio/AsioServer.h"
#include "asio/AsioSocketSession.h"
#include "asio/AsioCommon.h"

namespace                         spo   {
namespace                         asio  {

//------------------------------------------------------------------------------

/**
 * @brief Класс AsioAcceptor определяет атрибуты и методы работы по обслуживанию
 *        подключения клиентов к конечной точке.
 *
 * Параметры шаблона:
 * @value ProtocolT_ тип протокола обмена информации по IP-адресу сервера.
 * @value ByteT_ тип единицы информации для каналов обмена данными.
 *
 * Методы класса используют технологию Boost.Coroutine реализации алгоритмов на
 * основе сопрограмм.
 *
 */
template< typename                ByteT_ >
class SPO_CORE_EXPORT             AsioAcceptor :
public                            std::enable_shared_from_this< spo::asio::AsioAcceptor< ByteT_ > >
{
public:
  using self_t                  = spo::asio::AsioAcceptor< ByteT_ >;
  using service_t               = spo::asio::AsioService;
  using shared_t                = std::enable_shared_from_this< spo::asio::AsioAcceptor< ByteT_ > >;
  using session_t               = spo::asio::AsioSocketSession< spo::asio::tcp_t, ByteT_ >;

  /**
   * @brief  Конструктор AsioAcceptor формирует экземпляр класса для
   *            прослушивания TCP-подключений по сети к серверу по назначенному
   *            порту.
   * @param service ссылка на экземпляр сервиса обслуживания типа @a boost::asio::io_service
   * @param port порт TCP-сервера
   * @param protocol протокол для передачи данных. По умолчанию -
   */
  /**/                            AsioAcceptor
  (
      spo::asio::TransferType       type,
      const spo::asio::AsioServer< spo::asio::tcp_t, ByteT_ > & serverRef
  )
    : m_ServerRef   ( const_cast< spo::asio::AsioServer< spo::asio::tcp_t, ByteT_ > & >( serverRef ) )
    , m_Acceptor    ( self_t::service_t::Instance().ServiceRef() )
  {
    self_t::service_t::Instance().AddBeforeStartCallback
        ( {
            [ type, this ](void * ptr)
            {
              if( nullptr != ptr )
              {
                reinterpret_cast< self_t* >(ptr)->StartAcceptor( type );
              }
            },
            this
          } );
    self_t::service_t::Instance().AddBeforeStopCallback
        ( {
            [this](void * ptr)
            {
              if( nullptr != ptr )
              {
                reinterpret_cast< self_t* >(ptr)->StopAcceptor();
              }
            },
            this
          } );
  }

  /**
   * @brief Метод ServiceRef возвращает ссылку на сервис ввода/вывода Boost.Asio
   * @return ссылка на сервис ввода/вывода типа @a boost::asio::io_service библиотеки Boost.Asio
   */
  io_service_t                  & ServiceRef          ()
  {
    return std::ref( m_Acceptor.get_io_service() );
  }

  /**
   * @brief Метод IsOpen возвращает признак активности аксептора @a m_Acceptor.
   * @return Булево значение:
   * @value true акцептор активен и принимает подключения к серверу;
   * @value false акцептор неактивен, сервис @a ServiceRef() неактивен.
   */
  bool IsOpen () const BOOST_NOEXCEPT
  {
    return m_Acceptor.is_open();
  }

  /**
   * @brief Метод StartAcceptor пытается активировать работу приема/передачи
   *        данных для атрибута @a m_Acceptor
   * @param type тип (режим) работы обслуживания подключений
   * @see spo::asio::TransferType
   */
  void StartAcceptor ( spo::asio::TransferType type )
  {
    if( TryOpen() )
    try
    {
      SetOptions();
      boost::asio::spawn(
            io_strand_t( ServiceRef() ),
            boost::bind( & self_t::AcceptorAction, this, type, _1 ) );
    }
    catch ( const std::exception & e)
    {
      DUMP_EXCEPTION( e );
    }
  }

  /**
   * @brief Метод StopAcceptor производит попытки останова обслуживания подключений
   * @param tryStopCount количество попыток останова
   */
  void StopAcceptor ( short tryStopCount = 3 )
  {
    while( IsOpen() and ( tryStopCount-- > 0 ) )
    {
      if( TryClose() )
      {
        break;
      }
    }
    if( not IsOpen() )
      m_ServerRef.SetSocketsCount( 0 );
  }

private:
  spo::asio::AsioServer< spo::asio::tcp_t, ByteT_ > & m_ServerRef;
  /**
   * @brief Атрибут m_Acceptor содержит элемент обслуживания запроса от клиента
   *        на подключение к серверу.
   */
  typename tcp_t::acceptor        m_Acceptor;

  /**
   * @brief Метод TryOpen производит попытки активизации обслуживания подключения
   * @return Булево значение:
   * @value true  подключение прошло удачно;
   * @value false подключение не состоялось
   */
  bool  TryOpen ()
  {
    bool retval( IsOpen() );
    if( not retval )
      retval = m_ServerRef.SocketsValid();

    if( retval )
    {// создание и ассинхронный запуск задачи последовательной привязки прослушивания сокетов
      auto acceptor_future =
          std::async(
            std::launch::async,
            [ this ]()
            {
              bool success( false );
              spo::asio::error_t ec;
              try
              {
                while( ( not this->IsOpen() ) or ( not success ) )
                {
                  this->m_Acceptor.open( this->m_ServerRef.Protocol(), ec );
                  success = not spo::asio::AsioService::Instance().IsError( ec );
                  if( success )
                  {
                    this->SetOptions();
                    this->m_Acceptor.bind( this->m_ServerRef.Endpoint(), ec );
                    success = not spo::asio::AsioService::Instance().IsError( ec );
                    if( success )
                    {
                      this->m_Acceptor.listen( m_ServerRef.SocketsLimit(), ec );
                      success = not spo::asio::AsioService::Instance().IsError( ec );
                    }
                  }
                }
              }
              catch( const std::exception & e )
              {
                DUMP_EXCEPTION( e );
                success = false;
              }
              return success;
            } );

      // проверка готовности задачи выдать результат по истечении заданного
      // промежутка времени @a m_AcceptorTimeoutMs
      retval =
          acceptor_future.wait_for( std::chrono::milliseconds( m_ServerRef.TimeoutMs() ) )
          ==
          std::future_status::ready;

      if( retval )
        retval = acceptor_future.get();
    }
    return retval;
  }

  /**
   * @brief Метод TryClose производит попытки завершения обслуживания подключения
   * @return Булево значение:
   * @value true  удачное завершение обслуживания подключений к сокетам;
   * @value false завершение не состоялось
   */
  bool TryClose ()
  {
    // создание и ассинхронный запуск задачи отключения
    auto acceptor_future =
        std::async
        (
          std::launch::async,
          [ this ]()
          {
            boost::system::error_code ec;
            while( this->IsOpen() and ( IsNoErr( ec ) ) )
            { // выполнять попытки, пока обслуживание активно и нет ошибок отключения
              try
              {
                this->m_Acceptor.cancel( ec );
                this->m_Acceptor.close( ec );
              }
              catch( std::exception & e )
              {
                DUMP_EXCEPTION( e );
                break;
              }
            }
          }
        );

    // проверка готовности задачи выдать результат по истечении заданного
    // промежутка времени @a m_AcceptorTimeoutMs
    if( acceptor_future.wait_for( std::chrono::milliseconds( m_ServerRef.TimeoutMs() ) )
        ==
        std::future_status::ready )
      acceptor_future.get();

    return not IsOpen() ;
  }

protected:
  /**
   * @brief Защищенный виртуальный метод SetOptions задаёт опции для сокете по умолчанию.
   */
  virtual void SetOptions ()
  {
    using namespace spo::asio;
    error_t ec;
    acceptor_t::reuse_address o_reuse(true);
    acceptor_t::enable_connection_aborted o_aborted(true);
    m_Acceptor.set_option(o_reuse, ec);
    m_Acceptor.set_option(o_aborted, ec);
  }

  /**
   * @brief Метод AcceptorAction реализует действие при подключении очередного
   *        клиента к серверу
   * @param type тип (режим) работы сервера
   * @param yield контекст передачи управления очередной сопрограмме
   */
  void AcceptorAction ( TransferType type, boost::asio::yield_context yield )
  {
    while( IsOpen() )
    { // назначение сокета для нового подключения
      if( not m_ServerRef.SocketsValid() )
      {
        AsioService::Instance().SetState( AsioState::ErrSocketCount );
        return;
      }
      spo::asio::error_t ec;
      tcp_t::socket socket( ServiceRef() );
      m_Acceptor.async_accept( socket, yield[ ec ] );

      if( not spo::asio::AsioService::Instance().IsError( ec ) )
      { // подключение прошло успешно. Создание ощедоступного указателя на
        // экземпляр сессии работы с сокетом
        m_ServerRef.IncSocketsCount();
        auto session_ptr( std::move( MakeSocketSession< boost::asio::ip::tcp, ByteT_ >(
                                      m_ServerRef.ActionsRef(),
                                      type,
                                      std::move( socket ),
                                      m_ServerRef.SocketDeadline() ) ) );
        if( session_ptr )
        {
          session_ptr->SetAfterStop(
                []( void * ptr )
                {
                  auto s_ptr( reinterpret_cast< spo::asio::AsioServer< spo::asio::tcp_t, ByteT_ > * >(ptr) );
                  if( nullptr != s_ptr )
                  {
                    s_ptr->DecSocketsCount();
                  }
                }, & m_ServerRef );
          AsioService::Instance().ServiceRef().post(
            boost::bind(
              & spo::asio::AsioSocketSession< boost::asio::ip::tcp, ByteT_ >::Start,
              session_ptr ) );
        }
      }
    }
  }
};

}// namespace                     asio
}// namespace                     spo

#endif // ASIO_ACCEPTOR_H
