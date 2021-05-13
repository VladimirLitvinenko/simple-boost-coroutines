#ifndef IDOCUMENT_H
#define IDOCUMENT_H

#include "core/admin/AdminGlobal.h"
#include "MainLiterals.h"
#include "core/documents/DocumentLiterals.h"
#include "core/utils//CoreUtils.h"

#include <mutex>

#include <memory>
#include <map>
#include <functional>
#include <fstream>
#include <boost/filesystem.hpp>

namespace                       spo   {
namespace                       core  {
namespace                       docs  {

//------------------------------------------------------------------------------
class SPO_CORE_EXPORT           IDocument
{
public:
  /**
   * @brief Конструктор IDocument по умолчанию
   */
  /**/                          IDocument           () = default;
  /**
   * @brief Конструктор IDocument назначения идентификатора документу.
   * @param id
   */
  explicit                      IDocument           ( int id )
    : mID     ( id )
  {}
  /**
   * @brief Конструктор IDocument копирования
   * @param d
   */
  /**/                          IDocument           ( const IDocument & d )
  {
    * this = d;
  }
  /**
   * @brief Конструктор IDocument перемещения
   * @param d
   */
  /**/                          IDocument           ( IDocument && d      )
  {
    * this = std::move( d );
  }
  /**
   * @brief Дестркутор ~IDocument
   */
  virtual                       ~IDocument          ()
  {

  }
  /**
   * @brief Метод IsValid
   * @return
   */
  virtual bool                  IsValid             () const
  {
    return mID > DOC_UNDEFINED_ID;
  }
  /**
   * @brief Оператор operator ()
   * @return
   */
  virtual IDocument &           operator ()         ()
  {
    return std::ref( * this );
  }
  /**
   * @brief Оператор operator =
   * @param d
   * @return
   */
  virtual IDocument &           operator =          ( const IDocument & d )
  {
    BEGIN_LOCK_SECTION_SELF_;
      mID = d.GetID();
      return *this;
    END_LOCK_SECTION_
  }
  /**
   * @brief Оператор operator =
   * @param d
   * @return
   */
  virtual IDocument &           operator =          ( IDocument && d )
  {
    BEGIN_LOCK_SECTION_SELF_;
      mID = d.GetID();
      d.mID = DOC_UNDEFINED_ID;
      return *this;
    END_LOCK_SECTION_
  }
  /**
   * @brief Метод GetID
   * @return
   */
  int                           GetID               ()  const
  {
    return mID;
  }
  ///
  /// \brief Абстрактные методы
  ///
  /// {
  ///
  /**
   * @brief Метод FromByteArray
   * @return
   */
  virtual bool                  FromByteArray       ( const std::string & ) = 0;
  /**
   * @brief Метод ToByteArray
   * @return
   */
  virtual std::string           ToByteArray         () const = 0;
  /**
   * @brief Метод ClearContent
   */
  virtual void                  ClearContent        () = 0;
  ///
  /// }
  ///

  std::recursive_mutex  & MutexRef()
  {
    return std::ref( m_Mutex );
  }

protected:
  /**
   * @brief Атрибут m_Mutex
   */
  mutable std::recursive_mutex  m_Mutex;

  /**
   * @brief Метод GetIDRef
   * @return
   */
  int &                         GetIDRef            ()
  {
    return std::ref( mID );
  }

private:
  /**
   * @brief Атрибут mID содержит идентификатор типа документа
   */
  int                           mID                 = DOC_UNDEFINED_ID;
};

//------------------------------------------------------------------------------

using document_shared_t      = std::shared_ptr< spo::core::docs::IDocument >;
using document_unique_t      = std::unique_ptr< spo::core::docs::IDocument >;

//------------------------------------------------------------------------------
template
<
    typename  ContentT_,
    typename  = typename std::enable_if
                <
                  ( not std::is_void< ContentT_ >::value )
                  and
                  (
                    ( std::is_copy_assignable< ContentT_ >::value )
                    or
                    ( std::is_move_assignable< ContentT_ >::value )
                  )
                >
>
class SPO_CORE_EXPORT           Document :
public                          IDocument
{
public:
  using   base_class_t        = IDocument;
  using   element_t           = ContentT_;

protected:
  mutable typename Document::element_t  mContent;

public:
  /**/                          Document() = delete;
  /**/                          Document( int identificator )
    : Document::base_class_t( identificator )
  {}
  /**/                          Document( const Document & document )
    : Document::base_class_t()
  {
    * this = document;
  }
  /**/                          Document( Document && document )
    : Document::base_class_t()
  {
    *this = std::move( document );
  }
  /**/                          Document( const typename Document::element_t & cnt )
    : Document::base_class_t()
  {
    *this = cnt;
  }
  /**/                          Document( typename Document::element_t && cnt )
    : Document::base_class_t()
  {
    *this = std::move( cnt );
  }
  virtual                     ~ Document(){}

  typename Document::element_t  Content() const { return mContent; }

  virtual typename Document::element_t  &
  ContentRef()
  {
    return std::ref( mContent );
  }

  virtual typename Document::element_t *
  ContentPtr()
  {
    return std::forward< typename Document::element_t * >( & mContent );
  }

  ///
  /// \brief Метод SetContent
  /// \param content
  ///
  /// \warning Необходим коструктор присваивания ContentT_( const ContentT_ & )
  ///
  template
  <
      typename = typename std::enable_if
      <
        std::is_copy_assignable
        <
          typename Document::element_t
        >::value
      >
  >
  void SetContent( const typename Document::element_t & content ) BOOST_NOEXCEPT
  {
    BEGIN_LOCK_SECTION_SELF_
    mContent = std::forward< const typename Document::element_t & >( content );
    END_LOCK_SECTION_
  }

  template
  <
      typename = typename std::enable_if
      <
        std::is_copy_assignable
        <
          typename Document::element_t
        >::value
      >
  >
  void MoveContent( typename Document::element_t && content ) BOOST_NOEXCEPT
  {
    BEGIN_LOCK_SECTION_SELF_;
      mContent = std::move( std::forward< typename Document::element_t && >( content ) );
    END_LOCK_SECTION_
  }

  virtual Document & operator ()()
  {
    return std::ref( * this );
  }

  virtual Document & operator =( const std::string & data )
  {
    BEGIN_LOCK_SECTION_SELF_;
      FromByteArray( data );
      return std::ref( * this );
    END_LOCK_SECTION_
  }

  template
  <
      typename = typename std::enable_if
      <
        std::is_copy_assignable< typename Document::element_t >::value
      >
  >
  Document & operator =( const typename Document::element_t & content ) BOOST_NOEXCEPT
  {
    SetContent( content );
    return std::ref( * this );
  }

  template
  <
      typename = typename std::enable_if
      <
        std::is_copy_assignable< typename Document::element_t >::value
      >
  >
  Document & operator =( const Document & document ) BOOST_NOEXCEPT
  {
    BEGIN_LOCK_SECTION_SELF_;
      Document::base_class_t::operator =( static_cast< const IDocument &>(document) );
      SetContent( const_cast<Document &>(document).ContentRef() );
      return std::ref( * this );
    END_LOCK_SECTION_
  }

  template
  <
      typename = typename std::enable_if
      <
        std::is_move_assignable< typename Document::element_t >::value
      >
  >
  Document & operator =( Document && document ) BOOST_NOEXCEPT
  {
    BEGIN_LOCK_SECTION_SELF_;
      Document::base_class_t::operator =( std::move( static_cast< Document::base_class_t&& >( document ) ) );
      MoveContent( std::move( document.mContent ) );
      document.ClearContent();
      document.GetIDRef() = spo::core::docs::DOC_UNDEFINED_ID;

      return std::ref( * this );
    END_LOCK_SECTION_
  }

  template
  <
      typename = typename std::enable_if
      <
        std::is_move_assignable< typename Document::element_t >::value
      >
  >
  Document & operator =( typename Document::element_t && content ) BOOST_NOEXCEPT
  {
    BEGIN_LOCK_SECTION_SELF_;
      mContent = std::move( content );
      return std::ref( * this );
    END_LOCK_SECTION_
  }


  bool FromFile( const std::string & fileName )
  {
    std::ifstream read_stream( fileName.c_str() );
    bool retval( read_stream );
    if( retval  )
    {
      std::string data;
      std::getline( read_stream, data );
      retval = FromByteArray( data );
    }
    return retval;
  }

  bool ToFile( const std::string & fileName ) const
  {
    std::ofstream write_stream( fileName.c_str() );
    bool retval( write_stream );
    if( retval )
    {
      auto array( ToByteArray() );
      retval = write_stream.write( array.c_str(), array.size() );
    }
    return retval;
  }
};
//------------------------------------------------------------------------------

}// namespace                   docs
}// namespace                   core
}// namespace                   spo

#endif // IDOCUMENT_H
