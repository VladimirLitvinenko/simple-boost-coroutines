#ifndef SPOLITERALS_H
#define SPOLITERALS_H

#ifdef QT_VERSION
#include <QDebug>
#endif

#include <boost/system/system_error.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <functional>
#include <memory>
#include <future>
#include <chrono>
#include <tuple>
#include <assert.h>
#include <errno.h>
namespace                       spo   {

//------------------------------------------------------------------------------
using strings_t               = std::vector< std::string >;

#ifndef DIR_SEPARATOR
#ifdef Q_OS_WIN
#define DIR_SEPARATOR QLatinChar('\\')
#else
#define DIR_SEPARATOR "/"
#endif
#endif

using file_path_t               = boost::filesystem::path;

//------------------------------------------------------------------------------
using errors_t                  = std::map< std::remove_reference< decltype(errno) >::type, std::string >;
const errors_t                    SocketErrorsInfo
{
  { 0               , "Ошибки отсутствуют." },
  { ENOENT          , "Отсутствует файл или каталог." },
  { EACCES          , "Нет прав на создание сокета указанного типа и/или протокола." },
  { EPERM           , "Пользователь  попытался  соединиться  с  широковещательным  адресом,  не  установив  широковещательный  флаг на сокете или же запрос на соединение завершился неудачно из-за правила локального межсетевого экрана." },
  { EAFNOSUPPORT    , "Реализация не поддерживает указанное семейства адресов или Адрес имеет некорректное семейство адресов в поле sa_family." },
  { EINVAL          , "Неверные флаги в type, или Неизвестный протокол, или недоступное семейство протоколов." },
  { EMFILE          , "Переполнение таблицы файлов процесса." },
  { ENFILE          , "Достигнуто максимальное количество открытых файлов в системе." },
  { ENOBUFS         , "Недостаточно памяти для создания сокета. Сокет не может быть создан, пока не будет освобождено достаточное количество ресурсов." },
  { ENOMEM          , "Недостаточно памяти для создания сокета. Сокет не может быть создан, пока не будет освобождено достаточное количество ресурсов." },
  { EPROTONOSUPPORT , "Тип протокола или указанный протокол не поддерживаются в этом домене." },
  { EADDRINUSE      , "Локальный адрес уже используется." },
  { EADDRNOTAVAIL   , "(доменные сокеты Интернета) Сокет, указанный sockfd, ранее не был привязан к адресу и при попытке привязать его к эфемеридному порту, было определено, что все номера в  диапазоне  эфемеридных портов уже используются." },
  { EAGAIN          , "Недостаточно элементов в кэше маршрутизации." },
  { EALREADY        , "Сокет является неблокирующим, а предыдущая попытка установить соединение ещё не завершилась." },
  { EBADF           , "Файловый дескриптор не является правильными индексом в таблице дескрипторов." },
  { ECONNREFUSED    , "Ничто не слушает удалённый адрес." },
  { EFAULT          , "Адрес структуры сокета находится за пределами пользовательского адресного пространства." },
  { EINPROGRESS     , "Сокет  является неблокирующим, а соединение не может быть установлено немедленно. Можно использовать select(2) или poll(2), чтобы закончить соединение, установив ожидание возможности записи в"
"сокет. После того, как select(2) сообщит о такой возможности, используйте getsockopt(2), чтобы прочитать флаг SO_ERROR на уровне SOL_SOCKET, чтобы определить, успешно ли завершился  connect()"
"(в этом случае SO_ERROR равен нулю) или неудачно (тогда SO_ERROR равен одному из обычных кодов ошибок, перечисленных здесь, и объясняет причину неудачи)." },
  { EINTR           , "Системный вызов был прерван пойманным сигналом; см. signal(7)." },
  { EISCONN         , "Соединение на сокете уже произошло." },
  { ENETUNREACH     , "Сеть недоступна." },
  { ENOTSOCK        , "Файловый дескриптор не связан с сокетом." },
  { EPROTOTYPE      , "Тип сокета не поддерживается запрошенным протоколом связи. Это ошибка может возникать при попытке подключить доменнный датаграммный сокет UNIX к потоковому сокету." },
  { ETIMEDOUT       , "Произошел  тайм-аут  во время ожидания соединения. Сервер, возможно, очень занят и не может принимать новые соединения. Заметьте, что для IP-сокетов тайм-аут может быть очень длинным, если на сервере разрешено использование syncookies." },
  { ELOOP           , "При определении addr превышено количество переходов по символьной ссылке." },
  { ENAMETOOLONG    , "Аргумент addr слишком большой." },
  { ENOTDIR         , "Компонент в префиксе пути не является каталогом." },
  { EROFS           , "Попытка создания inode сокета на файловой системе, доступной только для чтения." },
  { ENXIO           , "Нет такого устройства или адреса" },
};

//------------------------------------------------------------------------------
const std::string               PROC_DIRECTORY          = "/proc/";
const std::string               PROC_CMDLINE_FILE       = "/cmdline";
const std::string               ETC_HOSTS_FILE          = "/etc/hosts";

const std::string               USER_ROOT               = "root";

const std::string               TMP_PATH                = "/tmp";
const std::string               PID_PATH                = "/run";
const std::string               LOG_PATH                = "/var/log";

const std::string               DATETIME_STR_FORMAT     = "dd.MM.yyyy HH.mm.ss";
const std::string               NET_MAC_STR_FORMAT      = "%02x-%02x-%02x-%02x-%02x-%02x";

const std::string               PKORI_PATH              = "/usr/local/pkori";
const std::string               PKORI_CONFIG_PATH       = spo::PKORI_PATH + DIR_SEPARATOR + "conf";
const std::string               ADMIN_PATH              = "/usr/local/salaga/adm";
const std::string               ADMIN_CONFIG_PATH       = spo::ADMIN_PATH + DIR_SEPARATOR + "conf";
const std::string               PKORI_IVSCONFIG_FILE    = "ivs.json";
const std::string               PKORI_LOCALCONFIG_FILE  = "node_passport.json";
const std::string               ARM_CONFIG_FILE         = "arm.conf";
const std::string               DIC_SETTINGS_FILE       = "gostsum";
const std::string               DIC_SUM_FILEEXT         = ".sum";
const std::string               EXT_FILE_CFG            = ".conf";

const std::string               PATH_OF_ADMIN           = "/usr/local/salaga";
const std::string               PATH_OF_ARM             = "/usr/local/pkoriarm";
const std::string               PATH_OF_PKORI           = "/usr/local/pkori";
const std::string               PATH_OF_BOOT            = "/boot";
const std::string               PATH_OF_OPT             = "/opt";
const std::string               AZIMUT_DATE_FILE        = "/tmp/sysdate.azimut";

const std::string               PING_CMD                = "ping";

const std::string               SERVICE_NAME_MAIN       = "pkori.main.spo";
const std::string               SERVICE_NAME_DEVICES    = "pkori.devman.spo";
const std::string               SERVICE_NAME_NETWORK    = "pkori.net.spo";
const std::string               SERVICE_DESCR_MAIN      = "ПКОРИ БЗИ сервис";
const std::string               SERVICE_DESCR_DEVICES   = "ПКОРИ КУФ сервис";
const std::string               SERVICE_DESCR_NETWORK   = "ПКОРИ ТПО сервис";
const std::string               SERVICE_MSG_ON          = " запущен.";
const std::string               SERVICE_MSG_OFF         = " НЕ запущен!";
const std::string               SERVICE_MSG_MAIN_ON     = SERVICE_DESCR_MAIN    + SERVICE_MSG_ON;
const std::string               SERVICE_MSG_DEVICES_ON  = SERVICE_DESCR_DEVICES + SERVICE_MSG_ON;
const std::string               SERVICE_MSG_NETWORK_ON  = SERVICE_DESCR_NETWORK + SERVICE_MSG_ON;
const std::string               SERVICE_MSG_MAIN_OFF    = SERVICE_DESCR_MAIN    + SERVICE_MSG_OFF;
const std::string               SERVICE_MSG_DEVICES_OFF = SERVICE_DESCR_DEVICES + SERVICE_MSG_OFF;
const std::string               SERVICE_MSG_NETWORK_OFF = SERVICE_DESCR_NETWORK + SERVICE_MSG_OFF;

const std::string               ABSTRACT_OVERLOAD_MESSAGE = "Перегрузите метод!";

const std::string               WARNING_TITLE = "Внимание!";
const std::string               WARNING_APP_TITLE = "Вариант приложения \"%1\" запущен ранее.";
const std::string               WROMG_APP_TITLE   = "Вариант приложения не создан.";
const std::string               WARNING_BTN_TITLE = "Закрыть";

const std::string               ERROR_HOSTS       = "Содержимое файла /etc/hosts не соответствует требованию ТЗ. Перезапустите КУФ ТПО.";
const std::string               ERROR_OPENMP_APPLY= "Библиотека OpenMP не применима.";
const std::string               ERROR_NEW_PINGER  = "Ошибка формирования ICMP-пинга";
const std::string               ERROR_RUN_PINGER  = "Ошибка значения адреса для оьправки ICMP-пинга";

#ifndef ERROR_ABSTRACT_OVERLOAD
# define ERROR_ABSTRACT_OVERLOAD \
  assert( false/*, Q_FUNC_INFO, spo::ABSTRACT_OVERLOAD_MESSAGE.toStdString().data()*/ );
#endif

#ifndef DUMP_EXCEPTION
#ifdef QT_VERSION
#define DUMP_EXCEPTION(ExceptValue) \
  qCritical() << __FILE__ << ":" << __LINE__ << ": " << Q_FUNC_INFO << "\n\tEXCEPTION :" << ExceptValue.what();
#else
#define DUMP_EXCEPTION(ExceptValue) \
  std::cout <<  __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__  << "\n\tEXCEPTION :" << ExceptValue.what() << std::endl;
#endif
#endif

#ifndef DUMP_EXCEPTION_BOOST
#ifdef QT_VERSION
#define DUMP_EXCEPTION_BOOST(Error) \
  qCritical() << Q_FUNC_INFO << "\n\tEXCEPTION: code" << Error.code().value() << "(" << Error.what() << ")";
#else
#define DUMP_EXCEPTION_BOOST(Error) \
  std::cout <<  __PRETTY_FUNCTION__  << "\n\tEXCEPTION : code " << Error.code().value() <<  << "(" << Error.what() << ")" << std::endl;
#endif
#endif

#ifndef DUMP_BOOST_ERROR
#ifdef QT_VERSION
#define DUMP_BOOST_ERROR(Error) \
  qDebug() << Q_FUNC_INFO << "boost error:" << Error.value() << "(" << Error.message().c_str() << ")";
#else
#define DUMP_BOOST_ERROR(Error) \
  std::cout <<  __PRETTY_FUNCTION__  << " boost error: " << Error.value() << "(" << Error.message().c_str() << ")" << std::endl;
#endif
#endif

#ifndef DUMP_ERRNO
#ifdef QT_VERSION
#define DUMP_ERRNO \
  qCritical() << Q_FUNC_INFO << "errno:" << errno << "(" << ::strerror( errno ) << ")";
#else
#define DUMP_ERRNO \
  std::cout <<  __PRETTY_FUNCTION__  << " errno: " << errno << "(" << ::strerror( errno ) << ")" << std::endl;
#endif
#endif

#ifndef DUMP_CRITICAL
#ifdef QT_VERSION
#define DUMP_CRITICAL(msg) \
  qCritical() << Q_FUNC_INFO << msg;
#else
#define DUMP_CRITICAL(msg) \
  std::cout <<  __PRETTY_FUNCTION__  << msg << std::endl;
#endif
#endif

#ifndef DUMP_INFO
#ifdef QT_VERSION
#define DUMP_INFO(msg) \
  qInfo() << Q_FUNC_INFO << "\n\t" << msg;
#else
#define DUMP_INFO(msg) \
  std::cout <<  __PRETTY_FUNCTION__ << "\n\t" << msg.c_str() << std::endl;
#endif
#endif


#ifndef DUMP_INFO_STL
#define DUMP_INFO_STL(msg) \
  std::cout <<  __PRETTY_FUNCTION__ << "\n\t" << msg.c_str() << std::endl;
#endif

//------------------------------------------------------------------------------
using thread_t                = boost::thread;
using threadptr_t             = std::shared_ptr< thread_t >;


using shared_thread_t           = std::shared_ptr< boost::thread >;

template< typename Res_         = void >
using shared_future_t           = std::shared_ptr< std::future< Res_ > >;

/**
 * @brief Тип notify_pathes_t представляет набор (вектор) файлов или каталогов
 * @note  Используется для формирования, например, списка исключений каталогов
 *        и|или файлов.
 */
using notify_pathes_t           = std::vector< std::string >;

/**
 * @brief Тип devprop_t представляет карту (ассоц. контейнер) свойств устройств
 *        операционной системы.
 */
using devprop_t                 = std::map< std::string, std::string >;


template< typename ResT_, typename ... Args_ >
using simple_fnc_t              = std::function< ResT_ ( Args_... ) >;

/**
 * @brief Тип socket_byffer_t определяет тип буфера обмена данных.
 */
template< typename                ByteT_ = unsigned char >
using socket_byffer_t           = std::vector< ByteT_ >;

}// namespace                   spo

#endif // SPOLITERALS_H
