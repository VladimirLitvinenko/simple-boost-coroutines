/**
 * @file FilesAction.h
 *
 * @brief Файл содержит описания класса FilesAction управления действием при обнаружении файла.
 *
 * @author Владимир Н. Литвиненко.
 * @orhanization Санкт-Петербургский филиал ПАО "НПО "Стрела"
 * @copyright Все права защищены.
 */

#ifndef FILESACTION_H
#define FILESACTION_H

#include "core/admin/AdminGlobal.h"
#include "core/utils/CoreUtils.h"

#include <functional>
#include <memory>
#include <boost/filesystem.hpp>

namespace                       spo   {
namespace                       core  {
namespace                       admin {

/**
 * @brief Класс FilesAction определяет атрибуты и методы действий при обнаружении
 * файла с заданным значением информации о нем.
 *
 * Используется на уровне объявления атрибута в классе @a spo::bzi::FileIntegrityControl.
 *
 * @see spo::bzi::FileIntegrityControl
 */
class SPO_CORE_EXPORT           FilesAction
{
public:
  /**
   * @brief Тип callback_t определяет функтор для действием над файлом.
   *
   * функтор принимает атрибуты типа @a spo::core::utils::FileInfoExtended файла.
   * Атрибут может применятся для выборки запуска действия, отказа и прочих алгоритмов
   * обработки информации разработчика ПО.
   */
  using callback_t            = std::function< bool ( const boost::filesystem::path & ) >;

  /**
   * @brief Конструтор FilesAction по-умолчанию.
   * Инициализация значений атрибутов класса осуществляется на этапе из объявления.
   */
  /**/                          FilesAction           () = default;
  /**
   * @brief Конструтор копирования FilesAction.
   * @param a lvalue-константная ссылка на экземпляр класса для его копирования.
   */
  /**/                          FilesAction           ( const FilesAction & a );
  /**
   * @brief Конструтор перемещения FilesAction.
   * @param a rvalue-ссылка на экземпляр класса для его перемещения.
   */
  /**/                          FilesAction           ( FilesAction && a );
  /**
   * @brief Конструтор перемещения FilesAction.
   * @param callback lvalue-константная ссылка на внешний метод выполнения алгоритма
   *        при обнаружении файла.
   */
  explicit                      FilesAction           ( const callback_t & callback );
  virtual                     ~ FilesAction           ();

  /**
   * @brief Виртуальный метод operator = определения оператора копирования экземпляра класса.
   * @param a lvalue-константная ссылка на экземпляр класса для его копирования.
   * @return Ссылка на текущий экземпляр класса.
   */
  virtual FilesAction &         operator=             ( const FilesAction & a ) ;
  /**
   * @brief Виртуальный метод operator = определения оператора переиещения экземпляра класса.
   * @param a rvalue-ссылка на экземпляр класса для его перемещения.
   * @return Ссылка на текущий экземпляр класса.
   */
  virtual FilesAction &         operator=             ( FilesAction && a ) ;

  /**
   * @brief Метод IsBusy возвращает признак занятости обработкой информации
   *        экземпляра класса.
   * @return Булевы значения:
   * @value true экземпляр класса занят обработкой информации;
   * @value false экземпляр класса НЕ занят обработкой информации;
   */
  bool                          IsBusy                () const;
  /**
   * @brief Метод ResetBusy сбрасывает признак занятости обработкой в состояние
   *        "НЕ ЗАНЯТ". Обработка прекращается немедленно.
   */
  void                          ResetBusy             ();

  /**
   * @brief Метод SetCallback назначает функтор выполнения алгоритма разработчика
   * при обнаружении файла согласно установленным критериям.
   * @param callback константная ссылка на функтор.
   */
  void                          SetCallback           ( const callback_t & callback ) BOOST_NOEXCEPT;
  /**
   * @brief Метод FileActions запускает сканирование с рекурсивным вызовом для
   *        подкаталогов файловой системы из списка файлов и/или каталогов.
   * @param fileList список файлов и/или каталогов для проверки (сканирования);
   * @param filters список фильтров типа @a QDir::Filters для каталогов,
   *        применяемый при сканировании.
   *
   * При обнаружении файла во время сканирования с применением критериев фильтра
   * производится вызов внешнего метода, если он указан, из значения атрибута
   * @a mCallBack. Метод принимает информацию типа @a spo::core::utils::FileInfoExtended
   * о текущем файле. Функтор можно переназначить методом @a FilesAction::SetCallback.
   *
   */
  void                          FileActions           ( const spo::strings_t & fileList );
  /**
   * @brief Метод DirList возвращает копию списка просканированных каталогов.
   * @return Копия списка просканированных каталогов.
   */
  spo::strings_t                DirList               () const;
  /**
   * @brief Метод Exclude возвращает копию списка исключаемых из поиска файлов и
   *        каталогов.
   * @return Копия списка исключаемых из поиска файлов и  каталогов.
   */
  spo::strings_t                Exclude               () const;
  /**
   * @brief Метод SetExclude назначает список исключаемых из поиска файлов и
   *        каталогов.
   * @param lvalue-константная ссылка на список исключаемых из поиска файлов и
   *        каталогов.
   */
  void                          SetExclude            ( const spo::strings_t & e );
  /**
   * @brief Метод DirListClear принудительно очищает содержимое списка
   *        просканированных каталогов.
   */
  void                          DirListClear          ();
  /**
   * @brief Метод ExcludedClear принудительно очищает содержимое исключаемых из
   *        поиска файлов и каталогов.
   */
  void                          ExcludedClear         ();

private:
  /**
   * @brief Атрибут m_Busy содержит атомарное значение о занятости (true)
   *        экземпляром класса обработкой информации.
   */
  static std::atomic_bool       m_Busy;
  /**
   * @brief Атрибут m_DirList содержит список просканированных каталогов.
   *
   * При очередном (не рекурсивном) вызове метода @a FilesAction::FileActions,
   * содержимое списка очищается.
   */
  spo::strings_t               m_DirList;
  /**
   * @brief Атрибут m_Exclude список исключаемых из рекурсивной проверки каталогов и файлов.
   */
  spo::strings_t               m_Exclude;
  /**
   * @brief Атрибут mCallBack содержит функтор типа @a callback_t, вызываемый
   * при обнаружении очередного очередного файла.
   */
  callback_t                    mCallBack             = nullptr;
};

}// namespace                   admin
}// namespace                   core
}// namespace                   spo

#endif // FILESACTION_H
