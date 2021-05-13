/**
 * @file AdminGlobal.h
 *
 * @brief Файл содержит глобальные макросы для работы с проектами.
 *
 * @author Владимир Н. Литвиненко.
 * @orhanization Санкт-Петербургский филиал ПАО "НПО "Стрела"
 * @copyright Все права защищены.
 */

#ifndef SOSPOADMIN_GLOBAL_H
#define SOSPOADMIN_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QObject>

Q_DECLARE_METATYPE( std::string )

#if defined(SOSPO_CORE_LIBRARY)
#  define SPO_CORE_EXPORT Q_DECL_EXPORT
#else
#  define SPO_CORE_EXPORT Q_DECL_IMPORT
#endif

#if defined(SOSPO_UI_LIBRARY)
#  define UI_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define UI_SHARED_EXPORT Q_DECL_IMPORT
#endif

#ifndef UNUSED
#  define UNUSED(variable) (void)variable;
#endif

#ifndef BOOST_COROUTINE_NO_DEPRECATION_WARNING
#  define BOOST_COROUTINE_NO_DEPRECATION_WARNING 1
#endif

#ifndef BEGIN_LOCK_SECTION_
#  define BEGIN_LOCK_SECTION_(mutex_reference) \
std::unique_lock<std::recursive_mutex> l( mutex_reference );
#endif

#ifndef BEGIN_LOCK_SECTION_SELF_
#  define BEGIN_LOCK_SECTION_SELF_ BEGIN_LOCK_SECTION_( std::ref (this->m_Mutex ) );
#endif

#ifndef BEGIN_UNLOCK_SECTION_
#  define BEGIN_UNLOCK_SECTION_(mutex) \
mutex.unlock();
#endif

#ifndef BEGIN_UNLOCK_SECTION_SELF_
#  define BEGIN_UNLOCK_SECTION_SELF_ BEGIN_UNLOCK_SECTION_( this->m_Mutex )
#endif

#ifndef END_LOCK_SECTION_
#  define END_LOCK_SECTION_ \
UNUSED(l);
#endif

#endif // SOSPOADMIN_GLOBAL_H
