/**
  * @file InotifyManager.h
  *
  * @brief Файл назначения типов и классов для работы с сообщениями об изменении
  *        каталогов и файлов Linux на уровне ядпа.
  *
  * @warning  Необходимо установить достаточное значение обработчика сообщений
  *           уровня ядра Linux.
  *           Например, при наличии разработчика в группе astra-admin OС Astra Linux 1.5 SE,
  *           можно в файле проекта qmake добавить установку признака
  *           INOTIFY_COMPLETE=$$system('sudo sysctl -w fs.inotify.max_user_watches=65536')
  *
  * @warning  Необходимо установить библиотеку inotifytools. Например, для
  *           OС Astra Linux 1.5 SE выполнить:
  *           sudo apt-get install -fa inotifytools
  *           В файле проекта qmake необходимо добавить установку переменной
  *           LIBS qmake:
  *           LIBS += -linotifytools
  */
#ifndef INOTIFYMANAGER_H
#define INOTIFYMANAGER_H

#include "MainLiterals.h"
#include "core/admin/AdminGlobal.h"

#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>

#include <set>

namespace                         spo   {
namespace                         core  {
namespace                         watch {

//------------------------------------------------------------------------------

/**
 * @brief Класс перечислений NotifyFlag определяет флаги сообщений при изменении
 *        состояния файлов и каталогов.
 *
 *  \note В аргументе inotify_add_watch(2) mask и поле mask структуры inotify_event,
 *  возвращаемых при чтении файлового дескриптора inotify, содержатся битовые маски,
 *  определяющие  события inotify.
 *  Следующие биты могут быть заданы в mask при вызове inotify_add_watch(2) и
 *  возвращены в поле mask, возвращаемом read(2):
 *  \value IN_ACCESS (+) Был произведён доступ к файлу (например, read(2), execve(2)).
 *  \value IN_ATTRIB (*) Изменились  метаданные  —  например,  права доступа
 *  (например, chmod(2)), отметки времени (например, utimensat(2)),
 *  расширенные атрибуты (setxattr(2)), счётчик ссылок (начиная с Linux 2.6.25;
 *  например, для аргумента назначения link(2) и unlink(2)) и идентификатор
 *  пользователя/группы (например, chown(2)).
 *  \value IN_CLOSE_WRITE (+) Файл, открытый для записи, был закрыт.
 *  \value IN_CLOSE_NOWRITE (*) Файл или каталог, не открытый для записи, был закрыт.
 *  \value IN_CREATE (+) В отслеживаемом каталоге был создан файл/каталог
 *  (например, open(2) O_CREAT, mkdir(2), link(2), symlink(2), bind(2) для
 *  доменного сокета UNIX).
 *  \value IN_DELETE (+) В отслеживаемом каталоге был удалён файл/каталог.
 *  \value IN_DELETE_SELF Отслеживаемый файл/каталог был удалён (это событие
 *  также возникает, если объект перемещён в другую файловую систему, так как mv(1),
 *  фактически, копирует файл в другую файловую систему и удаляет его из исходной).
 *  Также затем будет создано событие IN_IGNORED для дескриптора сторожка.
 *  \value IN_MODIFY (+) Файл был изменён (например, write(2), truncate(2)).
 *  \value IN_MOVE_SELF Отслеживаемый файл/каталог был перемещён.
 *  \value IN_MOVED_FROM (+) При переименовании генерируется для каталога,
 *  содержащего старое имя файла.
 *  \value IN_MOVED_TO (+) При переименовании генерируется для каталога,
 *  содержащего новое имя файла.
 *  \value IN_OPEN (*) Файл или каталог был открыт.
 *
 *  При наблюдении за каталогом:
 *  *  события, помеченные звёздочкой (*), могут возникать как для самого
 *  каталога, так и для файлов в каталоге; и
 *  *  события, помеченные знаком плюс (+), могут возникать только для объектов
 *  внутри каталога (но не самого каталога).
 *
 *  Когда события генерируются для объектов внутри отслеживаемого каталога,
 *  поле name, возвращаемое в структуре inotify_event, хранит имя файла внутри
 *  этого каталога.
 *
 *  Макрос IN_ALL_EVENTS определён как битовая маска всех перечисленных выше
 *  событий. Данный макрос можно использовать в качестве аргумента mask в
 *  вызове inotify_add_watch(2).
 *
 *  Дополнительно, два удобных макроса:
 *  \value IN_MOVE То же, что и IN_MOVED_FROM | IN_MOVED_TO.
 *  \value IN_CLOSE То же, что и IN_CLOSE_WRITE | IN_CLOSE_NOWRITE.
 *
 *  Также, при вызове inotify_add_watch(2) в mask могут быть указаны следующие биты:
 *  \value IN_DONT_FOLLOW (начиная с Linux 2.6.15) Не разыменовывать pathname,
 *  если это символическая ссылка.
 *  \value IN_EXCL_UNLINK (начиная с Linux 2.6.36) По  умолчанию,  при  слежении
 *  за  событиями  для  потомков  каталога, события генерируются для потомков
 *  даже после того, как они будут удалены из каталога. Это может привести к
 *  большому количеству ненужных для приложения событий (например, если следить
 *  за /tmp, в котором многие приложения создают и сразу удаляют временные  файлы).
 *  Указание IN_EXCL_UNLINK изменяет поведение по умолчанию, и такие события
 *  не генерируются для потомков после удаления из отслеживаемого каталога.
 *  \value IN_MASK_ADD Если  экземпляр  слежения  уже существует для объекта
 *  файловой системы соответствующего pathname, то выполнять добавление (OR)
 *  событий в mask к маске слежения (вместо замены маски).
 *  \value IN_ONESHOT Отслеживать объект файловой системы, соответствующий
 *  pathname до одного события, затем удалить объект из списка слежения.
 *  \value IN_ONLYDIR (начиная с Linux 2.6.15) Следить за pathname, только если
 *  это каталог. Этот флаг предоставляет приложению бессостязательный способ
 *  убедиться, что отслеживаемый объект — каталог.
 *  \value IN_IGNORED Слежение было снято явно (inotify_rm_watch(2)) или
 *  автоматически (файл был удалён или размонтирована файловая система).
 *  Также смотрите ДЕФЕКТЫ.
 *  \value IN_ISDIR Объект этого события — каталог.
 *  \value IN_Q_OVERFLOW Переполнена очередь событий (для этого события значение
 *  wd равно -1).
 *  \value IN_UNMOUNT Файловая система, содержащая отслеживаемый объект, была
 *  размонтирована. Также, будет сгенерировано событие IN_IGNORED для дескриптора
 *  сторожка.
 *
 *  \note   Интерфейсы в /proc
 *  Для ограничения потребления inotify памяти ядра, можно использовать
 *  следующие интерфейсы:
 *  \value /proc/sys/fs/inotify/max_queued_events  Значение  в  этом  файле
 *  используется  когда  приложение  вызывает  inotify_init(2) для установки
 *  верхнего порога количества событий, которые могут поместиться в очередь
 *  соответствующего экземпляра inotify. События, превысившие это ограничение,
 *  отбрасываются, но событие IN_Q_OVERFLOW генерируется всегда.
 *  \value /proc/sys/fs/inotify/max_user_instances В этом файле задаётся
 *  ограничение на количество экземпляров inotify, которые могут быть созданы
 *  для одного реального идентификатора пользователя.
 *  \value /proc/sys/fs/inotify/max_user_watches В этом файле задаётся ограничение
 *  на количество сторожков, которые могут быть созданы для одного реального
 *  идентификатора пользователя.
 *
 * @see inotifytools/inotify.h
 */
enum class                        NotifyFlag : unsigned int
{
  Undefined                     = 0,
  Access                        = IN_ACCESS,
  Modify                        = IN_MODIFY,
  Attributes                    = IN_ATTRIB,
  CloseAfterWrite               = IN_CLOSE_WRITE,
  CloseAfterNoWrite             = IN_CLOSE_NOWRITE,
  Open                          = IN_OPEN,
  Close                         = IN_CLOSE,
  MovedFrom                     = IN_MOVED_FROM,
  MovedTo                       = IN_MOVED_TO,
  Create                        = IN_CREATE,
  Delete                        = IN_DELETE,
  DeleteSelf                    = IN_DELETE_SELF,
  MoveSelf                      = IN_MOVE_SELF,

  FileWasIgnored                = IN_IGNORED,
  IsDir                         = IN_ISDIR,
  Unmount                       = IN_UNMOUNT,
  Overflow                      = IN_Q_OVERFLOW,

  NonBlock                      = IN_NONBLOCK,
  All                           = IN_ALL_EVENTS,
};
using NotifyFlagset             = std::set< NotifyFlag >;

//------------------------------------------------------------------------------

using notify_error_t            = std::remove_reference< decltype(errno) >::type;
using notify_method_t           = spo::simple_fnc_t
                                  <
                                    void,
                                    const std::string &,
                                    const spo::core::watch::NotifyFlagset &
                                  >;

/**
 * @brief Тип notify_actions_t представляет контейнер распределения действий по
 *        изменению параметров каталога или файла множества типа @a NotifyFlagset
 *        библиотеки inotifytools
 */
using notify_actions_t          = std::vector
                                  <
                                    std::pair
                                    <
                                      std::string     ,  ///< название каталога или файла
                                      notify_method_t    ///< исполняемый метод при получении события
                                    >
                                  >;

//------------------------------------------------------------------------------
/**
 *
 */
using notify_error_handler_t  = spo::simple_fnc_t< void, notify_error_t, const std::string & >;
using notify_run_handler_t    = spo::simple_fnc_t< void >;
using notify_stop_handler_t   = spo::simple_fnc_t< void >;

typedef struct notify_handlers
{
  notify_error_handler_t        m_OnError;
  notify_run_handler_t          m_Run;
  notify_stop_handler_t         m_OnStop;
}
notify_handlers_t;

//------------------------------------------------------------------------------
/**
 * @brief Класс InotifyManager
 *
 * @see man(7) inotify
 *
 * @note: notify - наблюдает за событиями файловой системы.
 *  Программный  интерфейс inotify предоставляет механизм для слежения за событиями
 *  в файловой системе. Его можно использовать для слежения за отдельными файлами
 *  или каталогами. При слежении за каталогами inotify возвращает события как
 *  для самого каталога, так и для файлов внутри каталога.
 *
 *  В программный интерфейс входят следующие системные вызовы:
 *
 *  *  Вызов inotify_init(2) создаёт экземпляр inotify и возвращает файловый
 *  дескриптор, ссылающийся на экземпляр inotify. Более новый inotify_init1(2)
 *  подобен  inotify_init(2),  но имеет аргумент flags, который предоставляет
 *  доступ к некоторым дополнительным возможностям.
 *
 *  *  Вызов  inotify_add_watch(2)  изменяет  «список  наблюдения»,  связанный
 *  с  экземпляром inotify. Каждый элемент (сторожок (watch)) в списке задаёт
 *  путь к файлу или каталогу и некоторый набор событий, которые ядро должно
 *  отслеживать для файла, на который указывает этот путь.
 *  Вызов  inotify_add_watch(2)  или  создаёт  новый  сторожок,  или  изменяет
 *  существующий. Каждый сторожок имеет уникальный «дескриптор сторожка» — целое
 *  число, возвращаемое inotify_add_watch(2) при создании сторожка.
 *
 *  *  При  возникновении  событий  с  отслеживаемыми  файлами  и  каталогами,
 *  эти события становятся доступны приложению в виде структурированных данных,
 *  которые можно прочитать из файлового дескриптора inotify с помощью read(2)
 *  (смотрите ниже).
 *
 *  *  Вызов inotify_rm_watch(2) удаляет элемент из списка наблюдения inotify.
 *
 *  *  При закрытии (с помощью close(2)) файловых дескрипторов, ссылающихся на
 *  экземпляр inotify, этот объект и его ресурсы освобождаются для  повторного
 *  использования  ядром;  все связанные сторожки освобождаются автоматически.
 *
 *  При  корректном  программировании,  приложение  может  эффективно использовать
 *  inotify для слежения и кэширования состояния набора объектов файловой системы.
 *  Однако, в тщательно проработанных приложениях нужно предполагать наличие
 *  ошибок в логике слежения или состязательности,  описанных  далее,  которые
 *  могут  приводить  к  рассогласованности  кэша  с состоянием файловой системы.
 *  Вероятно, лучше сделать некоторую проверку и перестроить кэш при обнаружении
 *  рассогласованности.
 */
class SPO_CORE_EXPORT             InotifyManager
{
public:

  /**/                            InotifyManager        ();
  virtual                       ~ InotifyManager        ();

  virtual bool                    IsActive              () const;
  virtual bool                    IsValid               () const;
  NotifyFlagset                   Flags                 () const BOOST_NOEXCEPT;

  void                            SetErrorHandler       ( const notify_error_handler_t &  ) BOOST_NOEXCEPT;
  void                            SetRunHandler         ( const notify_run_handler_t &  ) BOOST_NOEXCEPT;
  void                            SetStopHandler        ( const notify_stop_handler_t &   ) BOOST_NOEXCEPT;

  notify_actions_t                Actions               () const;
  spo::strings_t                  Pathes                () const;

  /**
   * @brief Method Do
   * @param actions
   * @param flags
   * @param excluded
   * @param ignore_by_regex
   */
  virtual void                    Run
  (
      const notify_actions_t    & actions,
      const NotifyFlagset       & flags               = NotifyFlagset({ NotifyFlag::All }),
      const notify_pathes_t     & excluded            = notify_pathes_t(),
      const std::string         & ignore_by_regex     = ""
  );

  static std::string              DumpStdString         ( const NotifyFlagset & );
  static void                     DumpFlags             ( const NotifyFlagset & );

protected:
  notify_actions_t              & ActionsRef            ()
  {
    return std::ref( m_Actions );
  }

private:
  NotifyFlagset                   m_Flags               { NotifyFlag::All };
  notify_handlers_t               m_Handlers;
  notify_actions_t                m_Actions;

  bool                            Init
  (
      const notify_pathes_t     & excluded              = {},
      const std::string         & ignore_by_regex       = ""
  ) BOOST_NOEXCEPT;

  void                            ParceEvent            ( struct inotify_event * ) BOOST_NOEXCEPT;
  unsigned int                    PrepareKey            ();
  NotifyFlagset                   PrepareFlags          ( const decltype(inotify_event::mask ) mask );
};

}// namespace                     watch
}// namespace                     core

//------------------------------------------------------------------------------

using inotify_mgr_t             = core::watch::InotifyManager;

}// namespace                     spo

#endif // INOTIFYMANAGER_H
