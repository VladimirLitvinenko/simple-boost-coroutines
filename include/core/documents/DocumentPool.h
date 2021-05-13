#ifndef DOCUMENTPOOL_H
#define DOCUMENTPOOL_H

#include "core/documents/IDocument.h"
#include "core/utils/CoreUtils.h"
#include "MainLiterals.h"

#include <QDebug>
#include <unordered_map>
#include <functional>
#include <typeindex>

namespace                         spo   {
namespace                         core  {
namespace                         docs  {


//------------------------------------------------------------------------------
class SPO_CORE_EXPORT             DocumentPool;
class                             UniqDocument
{
  friend class                    DocumentPool;
public:
  using deleter_t               = std::function< void( IDocument * ) >;
  using document_t              = std::shared_ptr< IDocument >;

private:
  mutable document_t              mDocumentPtr;
  std::recursive_mutex            m_Mutex;

public:
  /**/                            UniqDocument() = default;
  /**/                            UniqDocument( const UniqDocument & doc)
  {
    *this = doc;
  }
  /**/                            UniqDocument( UniqDocument && doc)
  {
    *this = std::move( doc );
  }

  virtual UniqDocument          & operator = ( const UniqDocument & doc)
  {
    BEGIN_LOCK_SECTION_SELF_
      mDocumentPtr = doc.mDocumentPtr;
      return *this;
    END_LOCK_SECTION_
  }

  virtual UniqDocument &          operator =    ( UniqDocument && doc)
  {
    BEGIN_LOCK_SECTION_SELF_
      mDocumentPtr.swap( doc.mDocumentPtr );
      doc.Reset();
      return *this;
    END_LOCK_SECTION_
  }

  virtual                       ~ UniqDocument  ()  {}

  void                            Reset         ()
  {
    mDocumentPtr.reset();
  }

  template
  <
      typename T_,
      typename ... Args_,
      typename = typename std::enable_if< std::is_same< T_, IDocument >::value >
  >
  bool                            Make        ( Args_&&  ...params ) BOOST_NOEXCEPT
  {
    try
    {
      mDocumentPtr.reset( new T_( std::forward< Args_&& >(params)... ) );
    }
    catch( const std::exception & e )
    {
      mDocumentPtr.reset();
      DUMP_EXCEPTION( e );
    }
    return mDocumentPtr.operator bool();
  }

  template
  <
      typename T_,
      typename = typename std::enable_if< std::is_same< T_, IDocument >::value >
  >
  void                            Set         ( T_ * ptr = nullptr )
  {
    try
    {
      mDocumentPtr.reset( ptr );
    }
    catch( const std::exception & e )
    {
      mDocumentPtr.reset();
      DUMP_EXCEPTION( e );
    }
  }

  document_t &                    Ref         ()
  {
    return std::ref( mDocumentPtr );
  }

  IDocument *             Get         () const
  {
    return dynamic_cast< IDocument* >( std::weak_ptr<document_t::element_type>( mDocumentPtr).lock().get() );
  }
};

//------------------------------------------------------------------------------
using pool_t                    = std::map< int, UniqDocument >;

//------------------------------------------------------------------------------
class SPO_CORE_EXPORT             DocumentPool
{
private:
  mutable pool_t                  mPool;
  mutable std::recursive_mutex    m_Mutex;

public:
  /**/                            DocumentPool  () = default;
  /**/                            DocumentPool  ( const DocumentPool & lValue )
  {
    *this = lValue;
  }
  /**/                            DocumentPool  ( DocumentPool && rValue )
  {
    *this = std::move( rValue );
  }
  /**/                            DocumentPool  ( const pool_t & pool )
  {
    *this = pool;
  }
  /**/                            DocumentPool  ( pool_t && pool )
  {
    *this = std::move( pool );
  }
  virtual                       ~ DocumentPool  ()
  {
    ClearContent();
  }
  bool                            IsEmpty       ()  const
  {
    return mPool.empty();
  }
  std::size_t                     Size          ()  const
  {
    return mPool.size();
  }
  std::vector<int>                Keys          ()  const
  {
    std::vector< int > retval;
    retval.reserve( Size() );
    for( auto & reference : mPool )
    {
      retval.push_back( reference.first );
    }
    return retval;
  }

  std::pair< pool_t::iterator, bool > Get           ( int key )
  {
    std::pair< pool_t::iterator, bool > retval { mPool.end(), not IsEmpty() };
    if( retval.second )
    {
      retval.first  = mPool.find( key );
      retval.second = mPool.end() != retval.first;
    }
    return retval;
  }

  template
  <
      typename T_,
      typename ... Args_,
      typename = typename std::enable_if< std::is_same< T_, IDocument >::value >
  >
  std::pair< pool_t::iterator, bool > Make
  (
      typename pool_t::key_type       key,
      Args_                     &&... params
  )
  BOOST_NOEXCEPT
  {
    std::pair< pool_t::iterator, bool > retval{ mPool.end(), false };
    try
    {
      retval.second =
          mPool[ key ].Make< T_, Args_... >( std::forward< Args_&& >( params )... );
      if( retval.second )
      {
        retval.first  = mPool.find( key );
      }
    }
    catch( const std::exception & e)
    {
      retval.first = mPool.end();
      DUMP_EXCEPTION( e );
    }

    retval.second = retval.first != mPool.end();
    return retval;
  }

  std::pair< pool_t::iterator, bool > Append
  (
      typename pool_t::key_type   key,
      typename UniqDocument::document_t ptr
  ) BOOST_NOEXCEPT
  {
    std::pair< pool_t::iterator, bool > retval{ mPool.end(), false };
    try
    {
      mPool[ key ].Ref() = ptr;
      retval.first = mPool.find( key );
    }
    catch( const std::exception & e )
    {
      retval.first = mPool.end();
      DUMP_EXCEPTION( e );
    }
    retval.second = mPool.end() != retval.first;
    return retval;
  }

  virtual void                    ClearContent      ()
  {
    mPool.clear();
  }

  virtual DocumentPool          & operator=         ( const DocumentPool & lValue )
  {
    mPool = lValue.mPool;
    return * this;
  }

  virtual DocumentPool          & operator =        ( DocumentPool && rValue )
  {
    mPool.swap( std::move( rValue ).Ref() );
    return *this;
  }

  virtual DocumentPool          & operator =        ( const pool_t & pool )
  {
    BEGIN_LOCK_SECTION_SELF_
    mPool = pool;
    return *this;
    END_LOCK_SECTION_
  }

  virtual DocumentPool          & operator =        ( pool_t && pool )
  {
    BEGIN_LOCK_SECTION_SELF_
    mPool.swap( pool );
    pool.clear();
    return *this;
    END_LOCK_SECTION_
  }

  template
  <
    typename Deleter_ = std::default_delete< spo::core::docs::IDocument >
  >
  DocumentPool                  & operator <<     ( const typename pool_t::value_type & value )
  {
    auto & reference = const_cast< typename pool_t::value_type & >( value );
    Append/*< IDocument >*/( reference.first,
                         reference.second.Ref()  );
    return *this;
  }

  virtual DocumentPool          & operator ()     ()
  {
    return std::ref( * this );
  }

  virtual std::pair< pool_t::iterator, bool > operator()( int key )
  {
    return Get( key );
  }

  virtual spo::core::docs::pool_t::mapped_type & operator []( int key )
  {
    return std::ref( mPool.find( key )->second );
  }
};
//------------------------------------------------------------------------------

}// namespace                   docs
}// namespace                   core
}// namespace                   spo

#endif // DOCUMENTPOOL_H
