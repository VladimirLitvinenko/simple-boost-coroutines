/**
 * @file FilesAction.cpp
 *
 * @brief Файл содержит реализацию класса FilesAction управления действием при обнаружении файла.
 *
 * @author Владимир Н. Литвиненко.
 * @orhanization Санкт-Петербургский филиал ПАО "НПО "Стрела"
 * @copyright Все права защищены.
 */

#include "core/admin/FilesAction.h"
#include "core/utils/CoreUtils.h"
#include "MainLiterals.h"

#include <utility>
#include <thread>

namespace                       spo   {
namespace                       core  {
namespace                       admin {

using namespace spo::core::utils;

std::atomic_bool                FilesAction::m_Busy{ false };

FilesAction::FilesAction( const FilesAction& lVal )
  : FilesAction::FilesAction()
{
  * this = lVal;
}

FilesAction::FilesAction( FilesAction && rVal )
  : FilesAction::FilesAction()
{
  * this = std::move( rVal );
}

FilesAction::
FilesAction( const FilesAction::callback_t& callback )
{
  SetCallback( callback );
}

FilesAction::~FilesAction()
{

}

FilesAction &
FilesAction::operator=( const FilesAction& lVal )
{
  m_Busy        = lVal.IsBusy();
  m_DirList     = lVal.DirList();
  m_Exclude     = lVal.m_Exclude;
  mCallBack     = lVal.mCallBack;
  return * this;
}

FilesAction &
FilesAction::operator=( FilesAction&& rVal )
{
  m_DirList     . swap ( rVal.m_DirList );
  m_Exclude     . swap( rVal.m_Exclude );
  mCallBack     = std::move( rVal.mCallBack );
  m_Busy        = rVal.IsBusy();
  rVal          . ResetBusy();

  return * this;
}

bool
FilesAction::IsBusy() const
{
  return m_Busy;
}

void
FilesAction::ResetBusy()
{
  m_Busy = false;
}

void
FilesAction::SetCallback( const FilesAction::callback_t& callback )
BOOST_NOEXCEPT
{
  mCallBack = callback;
}

void
FilesAction:: FileActions( const spo::strings_t  & fileList )
{
  m_Busy = ( not fileList.empty() ) and ( nullptr != mCallBack );
  if(  ! IsBusy()  )
    return;

  try
  {
    spo::strings_t new_list( fileList );
    for( auto path_name : new_list )
    {
      if( not IsBusy() )
        return;

      using namespace spo::core::utils;
      auto                calc_pair   ( RecurciveCalc( path_name ) );
      if( not calc_pair.second )
        continue;
      boost::filesystem::path file_info   ( calc_pair.first );
      std::vector<boost::filesystem::path> dirs;

      if( not spo::core::admin::ExactMatch(
            boost::regex( spo::core::utils::StrJoin( m_Exclude,"") ),
            file_info.string() ) )
        continue;

      if( IsBusy()  )
      {
        if( boost::filesystem::is_directory( file_info ) )
        {
          dirs.push_back( file_info );
          m_DirList.push_back( file_info.string() );

          for( auto & dir : dirs )
          {
            for( auto i = boost::filesystem::directory_iterator( dir );
                 i != boost::filesystem::directory_iterator();
                 i++ )
            {
              if( IsBusy() )
                FileActions( spo::strings_t( { i->path().string() } ) );
            }
          }
        }
        else
        {
          mCallBack( file_info );
        }
        std::this_thread::yield();
      }
      std::this_thread::yield();
    }
  }
  catch( const std::exception & e )
  {
    DUMP_EXCEPTION( e );
    ResetBusy();
  }
}

spo::strings_t
FilesAction::DirList() const
{
  return m_DirList;
}

spo::strings_t
FilesAction::Exclude() const
{
  return m_Exclude;
}

void
FilesAction::SetExclude(const spo::strings_t & re )
{
  m_Exclude = re;
}

void
FilesAction::DirListClear()
{
  m_DirList.clear();
}

void
FilesAction::ExcludedClear()
{
  m_Exclude.clear();
}

}// namespace                   admin
}// namespace                   core
}// namespace                   spo
