#ifndef SHARED_POOL_H
#define SHARED_POOL_H

#include "core/utils/SharedInfo.h"
#include <QString>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <functional>

namespace                       spo   {
namespace                       core  {
namespace                       utils {

//------------------------------------------------------------------------------
template< typename              ClassT_ >
using sharedpoolelement_t     = std::shared_ptr< ClassT_ >;

template< typename              ClassT_ >
using sharedpool_t            = std::map< std::size_t, sharedpoolelement_t< ClassT_ > >;

template< typename              ClassT_ >
using sharedpoolvalue_t       = std::pair< std::size_t, sharedpoolelement_t< ClassT_ > >;

template< typename              ClassT_ >
using element_ptr_t           = ClassT_ *;

//------------------------------------------------------------------------------
using key_alloocator_t        = spo::simple_fnc_t< std::size_t >;

template< typename                ClassT_ >
class SPO_CORE_EXPORT             SharedPool :
public                            spo::core::utils::SharedInfo,
public                            boost::enable_shared_from_this< SharedPool< ClassT_ > >,
protected                         boost::noncopyable
{
public:
  using base_class_t          = spo::core::utils::SharedInfo;
  using class_t               = ClassT_;

  /**/                            SharedPool          ();
  /**/                            SharedPool          ( const SharedPool< ClassT_ > & ) = delete;
  /**/                            SharedPool          ( SharedPool< ClassT_ > && );
  explicit                        SharedPool          ( SharedPool< ClassT_ >::status_handler_t handler );
  virtual                         ~SharedPool         ();

  virtual SharedPool<ClassT_> &   operator=           ( SharedPool< ClassT_ > && );
  virtual SharedPool<ClassT_> &   operator=           ( std::initializer_list< ClassT_ * > );

  bool                            IsEmpty             () const;
  std::size_t                     Size                () const;

  inline
  void                            AssignKeyAllocator  ( const key_alloocator_t & a )
  {
    m_KeyAllocator = a;
  }
  inline bool                     KeyAllocatorExists  () const
  {
    return m_KeyAllocator.operator bool();
  }
  inline std::size_t              RunKeyAllocator     ()
  {
    return m_KeyAllocator ? m_KeyAllocator() : 0;
  }

  virtual
  sharedpoolelement_t< ClassT_ >  Insert              ( sharedpoolelement_t< ClassT_ > && element );

  virtual
  sharedpoolelement_t< ClassT_ >  Insert              ( ClassT_ * ptr )
  {
    return Insert( std::move( sharedpoolelement_t< ClassT_ >( ptr ) ) );
  }

  template< typename ... Args_ >
  inline
  sharedpoolelement_t< ClassT_ >  Insert              ( Args_ && ... params )
  {
    return Insert( new ClassT_( std::forward< Args_&&>( params )... ) );
  }

  template< typename ... Args_ >
  inline
  sharedpoolelement_t< ClassT_ >  Make                ( Args_ && ... params )
  {
    return Insert( std::forward< Args_&&>( params )... );
  }

  void                            Clear               ();
  virtual void                    Erase               ( const std::size_t & key, bool destroy = true );
  virtual void                    Erase               ( const ClassT_ * value_ptr, bool destroy = true )
  {
    Erase( reinterpret_cast<std::size_t>(value_ptr), destroy );
  }
  void                            ResetAll            ()
  {
    for( auto & refer : m_Pool )
    {
      while( refer.second )
      {
        refer.second.reset();
      }
    }
  }

  typename sharedpool_t< ClassT_ >::iterator
                                  Find                ( const std::size_t & key );
  typename sharedpool_t< ClassT_ >::iterator
                                  Find                ( const ClassT_ * value_ptr );
  typename sharedpool_t< ClassT_ >::mapped_type
                                  PoolObject          ( const std::size_t & key );

protected:
  sharedpool_t< ClassT_ >       & GetPoolRef          () ;
  std::recursive_mutex          & MutexRef            ()
  {
    return std::ref( m_Mutex );
  }

private:
  mutable std::recursive_mutex    m_Mutex;
  mutable sharedpool_t< ClassT_ > m_Pool;
  key_alloocator_t                m_KeyAllocator;
};

//------------------------------------------------------------------------------

template< typename ClassT_ >
sharedpool_t< ClassT_>&
SharedPool<ClassT_>::GetPoolRef()

{
  return std::ref( m_Pool );
}

template< typename ClassT_ >
SharedPool<ClassT_>::SharedPool()
  : SharedPool::base_class_t()
{}

template< typename ClassT_ >
SharedPool<ClassT_>::SharedPool( SharedPool<ClassT_> && rValue )
{
  * this = std::move( rValue );
}

template< typename ClassT_ >
SharedPool<ClassT_>::SharedPool ( SharedPool<ClassT_>::status_handler_t handler )
  : SharedPool<ClassT_>()
{
  SetStatusHandler( handler );
}

template< typename ClassT_ >
SharedPool<ClassT_>::~SharedPool()
{
  ResetAll();
  Clear();
}

template< typename ClassT_ >
SharedPool< ClassT_> &
SharedPool<ClassT_>::operator=( SharedPool< ClassT_ > && rValue )
{
  BEGIN_LOCK_SECTION_SELF_;
  {
    SharedPool<ClassT_>::base_class_t::operator =(
          std::move(
            static_cast<SharedPool<ClassT_>::base_class_t &&>(
              rValue ) ) );

    m_Pool.swap( rValue.m_Pool );
//    rValue.Clear();
  }
  return * this;
  END_LOCK_SECTION_;
}

template< typename ClassT_ >
SharedPool< ClassT_> &
SharedPool<ClassT_>::operator=( std::initializer_list< ClassT_ * > list )
{
  BEGIN_LOCK_SECTION_SELF_;
  {
    for( auto & ptr_ref : list )
      if( nullptr != ptr_ref )
        Insert( ptr_ref );
  }
  return * this;
  END_LOCK_SECTION_;
}

template< typename ClassT_ >
sharedpoolelement_t< ClassT_ >
SharedPool< ClassT_ >::Insert( sharedpoolelement_t< ClassT_ > && element)
{
  BEGIN_LOCK_SECTION_SELF_;
  try
  {
    if( element )
      SetStatus( InvalidArgument );

    if( Size() + 1 > MaxSize() )
      SetStatus( Status::Overflow );

    auto key( m_KeyAllocator ?
                m_KeyAllocator() :
                std::size_t( element.get() ) );
    auto iter( Find( key ) );
    if( iter == m_Pool.end() )
    {
      m_Pool[ key ] = std::move( element );
      SetStatus( Ok );
      return m_Pool.at( key );
    }
    return iter->second;
  }
  catch( std::exception & e)
  {
    DUMP_EXCEPTION( e );
    SetStatus( NoMemory );
  }
  return sharedpoolelement_t< ClassT_ >();
  END_LOCK_SECTION_;
}

template< typename ClassT_ >
void
SharedPool<ClassT_>::Clear()
{
  BEGIN_LOCK_SECTION_SELF_;
  m_Pool.clear();
  END_LOCK_SECTION_;
}

template< typename ClassT_ >
bool
SharedPool<ClassT_>::IsEmpty() const
{
  return m_Pool.empty();
}

template< typename ClassT_ >
std::size_t
SharedPool<ClassT_>::Size() const
{
  return m_Pool.size();
}

template< typename ClassT_ >
typename sharedpool_t< ClassT_ >::iterator
SharedPool< ClassT_ >::Find( const std::size_t & key )
{
  return
      m_Pool.empty() ?
        m_Pool.end() :
        m_Pool.find( key );
}

template< typename ClassT_ >
typename sharedpool_t< ClassT_ >::iterator
SharedPool< ClassT_ >::Find( const ClassT_ * ptr )
{
  return
      m_Pool.empty() ?
        m_Pool.end() :
        std::find_if( m_Pool.begin(),
                      m_Pool.end(),
                      [ this, &ptr ]( const typename sharedpool_t<ClassT_>::value_type & value )
  {
    return ptr == value.second.get();
  } );
}

template< typename ClassT_ >
typename sharedpool_t< ClassT_ >::mapped_type
SharedPool< ClassT_ >::PoolObject( const std::size_t& key )
{
  BEGIN_LOCK_SECTION_SELF_;
  auto iter( Find( key ) );
  return
      iter == m_Pool.end() ?
        typename sharedpool_t< ClassT_ >::mapped_type() :
        boost::weak_ptr< ClassT_ >( iter->second ).lock() ;
  END_LOCK_SECTION_;
}

template< typename ClassT_ >
void
SharedPool<ClassT_>::Erase(const std::size_t & key, bool destroy)
{
  if( not m_Pool.empty() )
  {
    BEGIN_LOCK_SECTION_SELF_;
    auto iter( Find( key ) );
    if( iter != m_Pool.end() )
    {
      if( destroy )
      {
        iter->second.reset();
      }
      m_Pool.erase( iter );
    }
    END_LOCK_SECTION_;
  }
}

}// namespace                   utils
}// namespace                   asio
}// namespace                   spo

#endif // SHARED_POOL_H
