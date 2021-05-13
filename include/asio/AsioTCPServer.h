#ifndef ASIOTCPSERVER_H
#define ASIOTCPSERVER_H

#include "asio/AsioServer.h"
#include "asio/AsioAcceptor.h"

namespace                         spo   {
namespace                         asio  {

//------------------------------------------------------------------------------

/**
 * @brief Класс AsioTCPServer определяет атрибуты и методы работы TCP сервера по
 *        приему-передаче данных по сети.
 *
 * Класс реализует ассинхронные прием и/или передачу данных между TCP-сервером и
 * подключенными клиентами.
 *
 * Тип шаблона указывает на единицу информации буфера обмена типа @a spo::socket_byffer_t
 * По-умолчанию, назначается тип unsigned char (byte).
 *
 * Сервер реализует обновременную передачу данных от клиента к серверу и наоборот.
 * Т.о., сервер может не обрабатывать принимаемые данные и постоянно отправлять
 * некоторые пакеты к вновь подключенным клиентам: находится в активном
 * режиме передачи.
 * Сервер может и не передавать ничего по запросам клиентов, в то же время постоянно
 * принимать данные от них.
 *
 * Данные режимы реализуются алгоритмами в подключаемых обработчиках
 * буферов вода и вывода методом @a AsioTCPServer::SetBufferAction. Например,
 * если метод типа @a spo::asio::DataType::Input приема данных не назначен, а
 * метод типа @a spo::asio::DataType::Output активен, то
 * сервер будет принимать данные, но не включит их обработчик.
 * Если, например,
 * метод типа @a spo::asio::DataType::Output реализует непрерывную цепочку данных
 * к передаче, сервер будет постоянно формировать буфер отправки и рассылать
 * содержимое всем подключенным клиентам.
 *
 * Обработка данных из буферов обмена производится поочередно: ЧТЕНИЕ (Receive)
 * и ЗАПИСЬ (Send).
 * Выполнение чтения и записи по сокету идут в ассинхронном режиме.
 *
 * Манипулируя алгоритмами методов и их отключения или подключения можно добиться
 * перевода TCP-сервера в режимы:
 * @value SimplexOutput метод непрерывной или с задержками передачи данных к клиентам;
 * @value SimplexInput метод непрерывной или с задержками обработки данных от клиентов;
 * @value HalfDuplex метод последовательного прима и передачи данных между клиентами и сервером;
 * @value FullDuplex метод одновременных прима и передачи данных между клиентами и сервером;
 *
 * Методы класса используют технологию Boost.Coroutine реализации алгоритмов на
 * основе сопрограмм.
 *
 */
template< typename                ByteT_ >
class SPO_CORE_EXPORT             AsioTCPServer :
public                            spo::asio::AsioServer< boost::asio::ip::tcp, ByteT_ >
{
public:
  using self_t                  = spo::asio::AsioTCPServer< ByteT_ >;
  using tcp_t                   = boost::asio::ip::tcp;
  using base_class_t            = spo::asio::AsioServer< self_t::tcp_t, ByteT_ >;
  using self_acceptor_t         = std::shared_ptr< spo::asio::AsioAcceptor< ByteT_> >;

private:
  mutable self_acceptor_t         m_Acceptor;

public:
  /**
   * @brief Конструктор AsioTCPServer формирует экземпляр класса для
   *        прослушивания TCP-подключений по сети к серверу по назначенному
   *        порту.
   * @param type тип (режим) работы сервера. Тип @a spo::asio::TransferType;
   * @param port порт TCP-сервера;
   * @param serviceTimeoutMs время ожидания активации сервиса типа @a boost::asio::io_service.
   */
  /**/                            AsioTCPServer
  (
      const spo::asio::TransferType   type,
      const spo::asio::port_t         port,
      const std::int64_t            & serviceTimeoutMs = 10000
  )
    : self_t::base_class_t( type, port, self_t::tcp_t::v4(), serviceTimeoutMs )
    , m_Acceptor          ( std::make_shared< typename self_t::self_acceptor_t::element_type >(
                              type,
                              std::ref( * this ) ) )
  {
  }

  bool IsValid () const BOOST_NOEXCEPT override
  {
    return
        self_t::base_class_t::IsValid()
        and
        m_Acceptor.operator bool();
  }

  /**
   * @brief Метод IsOpen сообщает о том, что сервер обслуживает подключение
   *        клиента (открыт)
   * @return Булево значение:
   * @value true идет обслуживание запроса клиента;
   * @value false подключение закрыто к запросам клиента.
   */
  bool IsOpen () const BOOST_NOEXCEPT
  {
    return
        IsValid()
        ? AcceptorRef()->IsOpen()
        : false;
  }

protected:
  /**
   * @brief Защищенный метод AcceptorRef выдает ссылку на атрибут типа
   *        @a AsioServer::self_acceptor_t обслуживания подключений.
   * @return ссылка на атрибут обслуживания подключений.
   */
  self_acceptor_t & AcceptorRef () const
  {
    return std::ref( m_Acceptor );
  }
};

//------------------------------------------------------------------------------

}// namespace                     asio
}// namespace                     spo

#endif // ASIOTCPSERVER_H
