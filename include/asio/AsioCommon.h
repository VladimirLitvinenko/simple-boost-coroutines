#ifndef ASIO_COMMON_H
#define ASIO_COMMON_H

//#define BOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
//#define BOOST_THREAD_PROVIDES_VARIADIC_THREAD

#ifndef BOOST_COROUTINE_NO_DEPRECATION_WARNING
# define BOOST_COROUTINE_NO_DEPRECATION_WARNING
#endif

#include "core/documents/IDocument.h"
#include "MainLiterals.h"
#include <functional>
#include <memory>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/function.hpp>
#include <boost/signals2.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/deadline_timer.hpp>


#ifdef QT_DEBUG
# include <QDebug>
#endif

namespace                         spo   {
namespace                         asio  {

//------------------------------------------------------------------------------

/**
 * @brief Класс-перечислене TransferType определяет тип (режим) обмена данными.
 */
enum class                        TransferType
{
  SimplexIn     = 0 , ///< одностороний прием данных (без ожидания и обработки ответа)
  SimplexOut        , ///< односторонняя передача данных (без ожидания и обработки према данных )
  HalfDuplexIn      , ///< двусторонний последовательный обмен данными ( прием затем передача )
  HalfDuplexOut     , ///< двусторонний последовательный обмен данными ( передача затем приём )
  FullDuplex        , ///< двусторонний ассинхронный (одновременный) обмен данными
};

/**
 * @brief Перечисление DataType определяет направление петока передачи данных.
 */
enum                              DataType : short
{
  Input                         = 0,  ///< передача на прием данных объектом
  Output                        = 1,  ///< передача на отправку данных объектом

  DataSize                      = 2,  ///< граница значений типа передачи данных
};

/**
 * @brief Метод ModeShort приводит к типу @a short значение типа @a spo::asio::DataType
 * @param mode приводимое значение типа @a spo::asio::DataType
 * @return результат типа @a short для @a type
 */
inline
short                             ModeShort( const spo::asio::DataType & mode )
  { return static_cast< short >( mode ); }

/**
 * @brief Метод ModeSizeT приводит к типу @a std::size_t значение типа
 *         @a spo::asio::DataType
 * @param mode приводимое значение типа @a spo::asio::DataType
 * @return результат типа @a std::size_t для @a type
 */
inline
std::size_t                      ModeSizeT( const spo::asio::DataType & mode )
  { return static_cast< std::size_t >( mode ); }

/**
 * @brief Константа ASIO_DEADLINE_STEPS_DEFAULT
 */
const std::size_t                 ASIO_DEADLINE_STEPS_DEFAULT ( 1 );
/**
 * @brief Константа ASIO_DEADLINE_DEFAULT
 */
const boost::int64_t              ASIO_DEADLINE_DEFAULT  ( 1000 );
/**
 * @brief Константа ASIO_DEADLINE_MILLISEC_TYPE
 */
const boost::uint64_t             ASIO_DEADLINE_MILLISEC_TYPE( 1000 );
/**
 * @brief Константа ASIO_DEADLINE_MICROSEC_TYPE
 */
const boost::uint64_t             ASIO_DEADLINE_MICROSEC_TYPE( 1000000 );
#if defined(BOOST_DATE_TIME_HAS_NANOSECONDS)
/**
 * @brief Константа ASIO_DEADLINE_NANOSEC_TYPE
 */
const boost::uint64_t             ASIO_DEADLINE_NANOSEC_TYPE( 1000000000 );
#endif

//template< boost::uint64_t         TimeOutDuration_ >
//using asio_timeout_t            = boost::date_time::subsecond_duration
//                                  <
//                                    boost::posix_time::time_duration,
//                                    TimeOutDuration_
//                                  >;

//------------------------------------------------------------------------------
/**
 * @brief Тип
 */
using error_t                   = boost::system::error_code;
/**
 * @brief Тип
 */
using asio_poolkey_t            = std::size_t;
/**
 * @brief Тип
 */
using io_service_t              = boost::asio::io_service;
/**
 * @brief Тип
 */
using icmp_t                    = boost::asio::ip::icmp;
/**
 * @brief Тип
 */
using tcp_t                     = boost::asio::ip::tcp;
/**
 * @brief Тип
 */
using udp_t                     = boost::asio::ip::udp;
/**
 * @brief Тип
 */
using icmp_resolver_t           = icmp_t::resolver;
/**
 * @brief Тип
 */
using icmp_socket_t             = icmp_t::socket;
/**
 * @brief Тип
 */
using tcp_socket_t              = tcp_t::socket;
/**
 * @brief Тип
 */
using udp_socket_t              = udp_t::socket;
/**
 * @brief Тип
 */
using io_serviceptr_t           = std::shared_ptr< io_service_t >;
/**
 * @brief Тип
 */
using io_servicewptr_t          = boost::weak_ptr< io_service_t >;
/**
 * @brief Тип
 */
using asio_signal_t             = boost::asio::signal_set;
using asio_signalptr_t          = std::shared_ptr< asio_signal_t >;
using asio_signal_action_t      = boost::function<error_t( int, error_t &) >;
using asio_signal_actions_t     = std::map< int/*signal number*/, asio_signal_action_t >;
using asio_tcpprotocol_t        = boost::asio::ip::tcp;
using io_serviceptr_t           = std::shared_ptr< io_service_t >;
using asio_serviceuptr_t        = std::unique_ptr< io_service_t >;
using asio_servicepool_t        = std::map< asio_poolkey_t/* address as std::size_t  value*/, io_serviceptr_t >;
using asio_poolmapped_t         = asio_servicepool_t::mapped_type;
using asio_servicepooliter_t    = asio_servicepool_t::iterator;
using asio_work_t               = boost::asio::io_service::work;
using asio_workptr_t            = std::shared_ptr< asio_work_t >;
using asio_workuptr_t           = std::shared_ptr< asio_work_t >;
using io_strand_t               = boost::asio::io_service::strand;
using asio_strandptr_t          = std::shared_ptr< io_strand_t >;
using asio_workpool_t           = std::map< asio_poolkey_t/* address as std::size_t  value*/, asio_workptr_t >;
using asio_workpooliter_t       = asio_workpool_t::iterator;
using asio_socket_tcp_t         = boost::asio::ip::tcp::socket;
using asio_deadline_timer_t     = boost::asio::deadline_timer;
using asio_deadline_ptr_t       = std::shared_ptr< asio_deadline_timer_t >;
using asio_steady_timer_t       = boost::asio::steady_timer;
using asio_steady_ptr_t         = std::shared_ptr< asio_steady_timer_t >;
using asio_socketptr_t          = std::shared_ptr< asio_socket_tcp_t >;
using asio_resolver_t           = boost::asio::ip::tcp::resolver;
using asio_resolverptr_t        = std::unique_ptr< asio_resolver_t >;
using asio_iterator_t           = asio_resolver_t::iterator;
using asio_query_t              = asio_resolver_t::query;
using asio_queryptr_t           = std::shared_ptr< asio_query_t >;
using port_t                    = unsigned short;
using duplex_ports_t            = std::vector< spo::asio::port_t >;
using acceptor_t                = boost::asio::ip::tcp::acceptor;
using asio_acceptor_ptr_t       = std::shared_ptr< acceptor_t >;
using asio_acceptor_uptr_t      = std::unique_ptr< acceptor_t >;
//using asio_endpoint_t           = boost::asio::ip::tcp::endpoint;
using asio_address_t            = boost::asio::ip::address;

using asio_pool_deleter_f       = boost::function< void( asio_poolkey_t ) >;

/**
 * @brief Тип endpoint_t определяет тип конечной точки подключения.
 */
template
<
    typename                      InternetProtocol,
    typename                    = typename std::enable_if
    <
      std::is_same< InternetProtocol, boost::asio::ip::tcp >::value
      or
      std::is_same< InternetProtocol, boost::asio::ip::udp >::value
      or
      std::is_same< InternetProtocol, boost::asio::ip::icmp >::value
    >::type
>
using endpoint_t                = boost::asio::ip::basic_endpoint< InternetProtocol >;

//------------------------------------------------------------------------------
using asio_errcallback_t        = spo::simple_fnc_t < void, const spo::asio::error_t & >;
using asio_errcallbacks_t       = std::map< boost::system::errc::errc_t, asio_errcallback_t >;

//------------------------------------------------------------------------------
inline
bool                              IsNoErr( const error_t & err )
  { return err.value() == boost::system::errc::success; }

inline
void DUMP_ASIO_ERROR( const error_t & error_code )
{
  std::cout << "Asio.Error: code =" << error_code.value()
            << " (" << error_code.message().c_str() << ")."
            << std::endl;
}

//------------------------------------------------------------------------------

}// namespace                     asio
}// namespace                     spo

/**
  * @a https://habr.com/ru/post/197392/
  * Информация отслеживания обработчиков
  *
  * Информацию не так легко понять, но, тем не менее она очень полезна.
  * На выходе Boost.Asio выдает следующее:
  *
  * @asio|<timestamp>|<action>|<description>.
  *
  * Первый тег всегда @asio, вы можете использовать его, чтобы легко фильтровать
  * сообщения, приходящие от Boost.Asio в случае, если другие источники пишут в
  * стандартный поток ошибок (эквивалент std::cerr). Экземпляр timestamp считается
  * в секундах и микросекундах, начиная с 1 января 1970 UTC.
  *
  * Экземпляр action может быть чем-то из следующего:
  *     >n: используется, когда мы входим в обработчик с номером n.
  *         Экземпляр description содержит аргументы, передаваемые обработчику.
  *     <n: используется, когда обработчик номер n закрывается.
  *     !n: используется, когда мы вышли из обработчика n из-за исключения.
  *     ~n: используется, когда обработчик с номером n разрушается без вызова;
  *         наверное, потому что экземпляр io_service уничтожается слишком рано
  *         (до того, как n получит шанс вызваться).
  *     n*m:используется, когда обработчик n создает новую асинхронную операцию
  *         с завершающим обработчиком под номером m. После старта запущенная
  *         асинхронная операция отобразится в экземпляре description.
  *         Завершающий обработчик вызовется, когда вы увидите >m(start) и <m(end).
  *     n:  используется, когда обработчик с номером n выполняет операцию, которая
  *         отображается в description (которая может быть close или операцией cancel).
  *         Обычно, вы можете смело их игнорировать.
  *
  * Всякий раз, когда n = 0, то снаружи выполняются все обработчики (асинхронно),
  * обычно, вы видите, когда выполняется первая операция (операции) или в случае,
  * если вы работаете с сигналами и срабатывает сигнал.
  *
  * Вы должны обращать внимание на сообщения типа !n и ~n, которые возникают,
  * когда есть ошибки в коде. В первом случае, асинхронная функция не выбросила
  * исключение, таким образом, исключение должно быть сгенерировано вами, вы не
  * должны допускать исключений при выходе из вашего завершающего обработчика.
  * В последнем случае вы, вероятно, уничтожили экземпляр io_service слишком
  * рано, до завершения всех вызванных обработчиков.
  *
  * Пример
  *
  * Для того, чтобы показать вам пример вспомогательной информации, позвольте
  * изменить пример из 6 главы. Все, что вам нужно сделать, это добавить дополнительный
  *
  * #define перед включением boost/asio.hpp:
  *
  * @code
  * #define BOOST_ASIO_ENABLE_HANDLER_TRACKING
  * #include <boost/asio.hpp>
  *
  * ...
  *
  * @endcode
  *
  * Так же мы выведем дамп в консоль, когда пользователь войдет в систему и
  * получит первый список клиентов. Вывод будет следующий:
  *
  * @asio|1355603116.602867|0*1|socket@008D4EF8.async_connect
  * @asio|1355603116.604867|>1|ec=system:0
  * @asio|1355603116.604867|1*2|socket@008D4EF8.async_send
  * @asio|1355603116.604867|<1|
  * @asio|1355603116.604867|>2|ec=system:0,bytes_transferred=11
  * @asio|1355603116.604867|2*3|socket@008D4EF8.async_receive
  * @asio|1355603116.604867|<2|
  * @asio|1355603116.605867|>3|ec=system:0,bytes_transferred=9
  * @asio|1355603116.605867|3*4|io_service@008D4BC8.post
  * @asio|1355603116.605867|<3|
  * @asio|1355603116.605867|>4|
  * John logged in
  * @asio|1355603116.606867|4*5|io_service@008D4BC8.post
  * @asio|1355603116.606867|<4|
  * @asio|1355603116.606867|>5|
  * @asio|1355603116.606867|5*6|socket@008D4EF8.async_send
  * @asio|1355603116.606867|<5|
  * @asio|1355603116.606867|>6|ec=system:0,bytes_transferred=12
  * @asio|1355603116.606867|6*7|socket@008D4EF8.async_receive
  * @asio|1355603116.606867|<6|
  * @asio|1355603116.606867|>7|ec=system:0,bytes_transferred=14
  * @asio|1355603116.606867|7*8|io_service@008D4BC8.post
  * @asio|1355603116.607867|<7|
  * @asio|1355603116.607867|>8|
  * John, new client list: John
  *
  * Позвольте проанализировать каждую строчку:
  *
  * Мы вводим async_connect, которая создает обработчик 1(в нашем случае все
  * обрабатывают talk_to_svr::step)
  * Вызывается обработчик 1 (после успешного подключения к серверу)
  * Обработчик 1 вызывает async_send, которая создает обработчик 2 (здесь мы
  * посылаем сообщение с логином на сервер)
  * Обработчик 1 закрывается
  * Вызывается обработчик 2 и посылает 11 байт (login John)
  *     Обработчик 2 вызывает async_receive, которая создает обработчик 3 (мы
  *     ждем, когда сервер ответит на наше сообщение с логином)
  *     Обработчик 2 закрывается
  *     Вызывается обработчик 3 и получает 9 байт (login ok)
  * Обработчик 3 перенаправляет в on_answer_from_server (где создается обработчик 4)
  * Обработчик 3 закрывается
  * Вызывается обработчик 4, который потом запишет в дамп John logged in
  *     Обработчик 4 запускает еще один step (обработчик 5), который будет писать ask_clients
  *     Обработчик 4 закрывается
  *     Открывается обработчик 5
  *     Обработчик 5, async_send ask_clients, создает обработчик 6
  *     Обработчик 5 закрывается
  *     Вводится обработчик 6 (мы успешно отправили ask_clients серверу)
  *     Обработчик 6 вызывает async_receive, которая создает обработчик 7 (мы
  *     ждем, когда сервер отправит нам список существующих клиентов)
  *     Обработчик 6 закрывается
  *     Вызывается обработчик 7, и мы принимаем список клиентов
  *     Обработчик 7 запускает on_answer_from_serve (где создается обработчик 8)
  *     Обработчик 7 закрывается
  *     Открывается обработчик 8, и в дамп записывается список клиентов (on_clients)
  *
  * Это займет некоторое время, чтобы привыкнуть, но, как только вы поймете это,
  * вы сможете изолировать выходные данные, в которых содержится проблема и находить
  * фактическую часть кода, которая должна быть исправлена.
  */

#endif // ASIO_COMMON_H
