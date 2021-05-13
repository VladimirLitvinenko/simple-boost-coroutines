#ifndef ASIORESOLVER_H
#define ASIORESOLVER_H

#include "asio/AsioService.h"
#include "asio/AsioSocketSession.h"

namespace                         spo   {
namespace                         asio  {

//------------------------------------------------------------------------------
/**
 * @brief Класс AsioResolver формирует правила и атрибуты для подключения к
 * серверу по проеделенному типу протокола и правилами подключения.
 *
 * Параметры шаблона:
 * @value ProtocolT_ тип протокола для обмена данными по сети.
 * @value ByteT_ тип единицы информации для каналов обмена данными.
 */
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
class SPO_CORE_EXPORT             AsioResolver :
public                            std::enable_shared_from_this< spo::asio::AsioResolver< ProtocolT_, ByteT_ > >
{
public:
  using self_t                  = spo::asio::AsioResolver< ProtocolT_, ByteT_ >;
  using shared_t                = std::enable_shared_from_this< spo::asio::AsioResolver< ProtocolT_, ByteT_ > >;
  using protocol_t              = ProtocolT_;
  using resolver_t              = typename protocol_t::resolver;
  using query_t                 = typename resolver_t::query;
  using iterator_t              = typename resolver_t::iterator;
  using endpoint_t              = typename protocol_t::endpoint;
  using endpoints_t             = std::vector< typename self_t::endpoint_t >;
  using session_t               = spo::asio::AsioSocketSession< ProtocolT_, ByteT_ >;

  explicit AsioResolver
  (
      const std::string   & remoute_service,
      const std::int64_t    serviceTimeoutMs,
      boost::asio::ip::resolver_query_base::flags resolve_flags =
      boost::asio::ip::resolver_query_base::flags::passive
      |
      boost::asio::ip::resolver_query_base::flags::address_configured
  )
    : m_Resolver( spo::asio::AsioService::Instance( serviceTimeoutMs ).ServiceRef() )
    , m_Query   ( remoute_service, resolve_flags )
  {}

  explicit AsioResolver
  (
      const std::string   & host,
      const std::string   & remoute_service,
      const std::int64_t    serviceTimeoutMs,
      boost::asio::ip::resolver_query_base::flags resolve_flags =
      boost::asio::ip::resolver_query_base::flags::passive
      |
      boost::asio::ip::resolver_query_base::flags::address_configured
  )
    : m_Resolver( spo::asio::AsioService::Instance( serviceTimeoutMs ).ServiceRef() )
    , m_Query   ( host, remoute_service, resolve_flags  )
  {}

  /**
   * @brief Метод Hints вовращает структуру, содержащую информацию об адресе
   *        сервиса провайдера.
   * @return информация об адресе сервиса провайдера.
   */
  boost::asio::detail::addrinfo_type & Hints    () const
  {
    return m_Query.hints();
  }

  /**
   * @brief Метод HosName возвращает имя хоста сервера к подключению
   * @return имя хоста сервера типа @a std::string
   */
  std::string                     HosName       () const
  {
    return m_Query.host_name();
  }

  /**
   * @brief   Метод RemouteService возвращает имя сервиса ассоциированного с
   *          запросом.
   * @return  имя сервиса
   */
  std::string RemouteService () const
  {
    return m_Query.service_name();
  }

  /**
   * @brief   Метод Resolver возвращает ссылку на атрибут ответчика.
   * @return  ссылка на атрибут @a m_Resolver ответчика.
   */
  typename self_t::resolver_t & Resolver ()
  {
    return std::ref( m_Resolver );
  }

  /**
   * @brief   Метод Query возвращает ссылку на атрибут очереди параметров
   *          подключения к серверу.
   * @return  ссылку на атрибут @a m_Query очереди.
   */
  typename self_t::query_t & Query ()
  {
    return std::ref( m_Query );
  }

  /**
   * @brief   Метод IsValid возвращает признак контроля к работе.
   * @return  Признак контроля к работе:
   * @value true  экземпляр класса готов к работе;
   * @value false экземпляр класса НЕ готов к работе;
   */
  bool IsValid ( bool rescan = false )
  {
    if( rescan )
      Scan();
    return not m_Endpoints.empty();
  }

  /**
   * @brief   Метод ServiceRef возвращает ссылку на сервис Boost.Asio обслуживания
   *          ввода/вывода.
   * @return  сервис Boost.Asio обслуживания ввода/вывода.
   */
  io_service_t & ServiceRef ()
  {
    return m_Resolver.get_io_service();
  }

  /**
   * @brief   Метод Scan выполняет проверку доступности удаленных провайдеров.
   *
   * Метод формирует новый список конечных точек доступа согласно параметрам
   * очереди запрсов на подключение.
   */
  void Scan () BOOST_NOEXCEPT
  {
    m_Endpoints.clear();
    error_t ec;
    auto iter( m_Resolver.resolve( m_Query, ec ) );
    if( not AsioService::Instance().IsError( ec ) )
    {
      typename resolver_t::iterator end;
      while( iter != end )
      {
        m_Endpoints.push_back( *iter );
        iter++;
      }
      return;
    }
    m_Endpoints.clear();
  }

  /**
   * @brief Метод Endpoints возвращает список конечных точек доступа к сервисам
   *        провайдера (сервера).
   * @param rescan  значение указывает на необходимость (true) сканирования очереди
   *                запросов перед формирование списка конечных точек доступа.
   * @return копия списка конечных точек доступа как значений атрибута @a m_Endpoints
   */
  typename self_t::endpoints_t Endpoints ( bool rescan = false )
  {
    if( rescan )
      Scan();

    return m_Endpoints;
  }

private:
  /**
   * @brief Атрибут m_Resolver отвечает за возможность разрешения доступа к удаленному серверу.
   * @see boost::asio::ip::basic_resolver
   */
  mutable resolver_t m_Resolver;

  /**
   * @brief Атрибут m_Query содержит запрос к @a m_Resolver для формирования
   *        конечных точек доступа
   * @see boost::asio::ip::basic_resolver::query
   */
  query_t m_Query;

  /**
   * @brief Атрибут m_Endpoints содержит список конечных точек доступа к
   * удаленным сервисам (серверам)
   */
    mutable typename self_t::endpoints_t m_Endpoints;
};

//------------------------------------------------------------------------------

}// namespace                     asio
}// namespace                     spo

#endif // ASIORESOLVER_H
