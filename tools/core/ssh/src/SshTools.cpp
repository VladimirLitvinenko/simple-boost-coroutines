#include "SshTools.h"
#include <QByteArray>
#include <QDebug>

namespace               sospo {
namespace               core  {
namespace               net {

/**/                    SshTools::SshTools(QObject* parent)
  : SshTools::base_class_t( parent )
{
  mConnected.store( false );
  mErrorCode.store( -1 );
}

SshTools::~SshTools()
{
  tryDisconnect();
}

bool                    SshTools::isValid() const
{
  return mSession.operator bool();
}

bool SshTools::isDataComplete() const
{
  return
      isValid() and
      ( ! mHost.isEmpty() ) and
      ( 0 != mPort )/* and
      ( ! mUser.isEmpty() ) and
      ( ! mPassword.isEmpty() )*/
      /* && mChannel*/;
}

QString                 SshTools::host() const
{
  return mHost;
}

void                    SshTools::setHost(const QString& h )
{
  QMutexLocker l( &mMutex );
  mHost = h;
  qDebug() << Q_FUNC_INFO << mHost.toStdString().c_str();
  setErrorCode( ssh_options_set( mSession.get(),
                                SSH_OPTIONS_HOST,
                                mHost.toStdString().data() ) );
  Q_UNUSED( l );
}

unsigned                SshTools::port() const
{
  return mPort;
}

void                    SshTools::setPort( unsigned p )
{
  QMutexLocker l( &mMutex );
  mPort = p;
  setErrorCode( ssh_options_set(mSession.get(), SSH_OPTIONS_PORT, &mPort ) );
  Q_UNUSED( l );
}

QString                 SshTools::user() const
{
  return mUser;
}

void                    SshTools::setUser(const QString& u)
{
  QMutexLocker l( &mMutex );
  mUser = u;
  setErrorCode( ssh_options_set(mSession.get(), SSH_OPTIONS_USER, mUser.toStdString().data() ) );
  Q_UNUSED( l );
}

QString                 SshTools::password() const
{
  return mPassword;
}

void                    SshTools::tryDisconnect()
{
  if( mSession )
  {
    if( isReady() )
    {
      ssh_channel_close( mChannel.get() );
      ssh_channel_send_eof( mChannel.get() );
      mChannel.reset();
    }
    mSession.reset();
  }
  mErrorCode = SSH_OK;
  mConnected = false;
}

QString                 SshTools::issueBanner() const
{
  return isValid() ? ssh_get_issue_banner( mSession.get() ) : "<error>";
}

int                     SshTools::errorCode() const
{
  return isValid() ? ssh_get_error_code( mSession.get() ) : SSH_ERROR;
}

QString                 SshTools::errorStr() const
{
  return isValid() ? QString(ssh_get_error( mSession.get() )) : "Session deleted...";
}

void SshTools::VerifyKnownHost()
{
  QMutexLocker l( &mMutex );
  if( ! isConnected() )
    return;

  unsigned char* hash;

  setErrorCode( ssh_is_server_known( mSession.get() ) );
  if( isError() )
    return;

  auto hlen( ssh_get_pubkey_hash( mSession.get(), &hash ) );
  if (hlen < 0)
  {
    setErrorCode( SSH_ERROR );
    return;
  }

  delete hash;

  switch ( mErrorCode.load() )
  {
    case SSH_SERVER_KNOWN_CHANGED:
    case SSH_SERVER_FOUND_OTHER:
    case SSH_SERVER_ERROR:
    {
      return;
    }
    case SSH_SERVER_FILE_NOT_FOUND:
    case SSH_SERVER_NOT_KNOWN:
    {
      setErrorCode( ssh_write_knownhost( mSession.get() ) );
      break;
    }
    default:; // SSH_SERVER_KNOWN_OK
  }
  Q_UNUSED( l );
}

void SshTools::Authenticate(const QString& remouteUser, const QString& remoutePasswd)
{
  if( isDataComplete() and isConnected() )
  {
    setUser     ( remouteUser );
    setPassword ( remoutePasswd );
    {
      QMutexLocker l( &mMutex );
      mChannel.reset( ssh_channel_new( mSession.get() ) );
      setErrorCode( ssh_userauth_password( mSession.get(),
                                           mUser.toStdString().data(),
                                           mPassword.toStdString().data() ) );
      Q_UNUSED( l );
    }
    if( ! isError() )
      setErrorCode( ssh_channel_open_session( mChannel.get() ) );
  }
}

void SshTools::setErrorCode(int code)
{
  mErrorCode = code;
  if( isError() )
  {
    qDebug() << "Error code: " << errorCode() << " = " << errorStr();
    tryDisconnect();
  }
}

bool SshTools::isConnected() const
{
  return isValid() and mConnected;
}

bool SshTools::isError() const
{
  return
      ( ! isValid() ) or
      (
        (SSH_OK !=  mErrorCode) and
        (SSH_SERVER_KNOWN_OK !=  mErrorCode) and
        ( SSH_AUTH_SUCCESS != mErrorCode )
        );
}

bool SshTools::isReady() const
{
  return isValid() and mChannel.operator bool();
}

int SshTools::verbosity() const
{
  return mVerbosity;
}

void SshTools::setVerbosity(int verbosity)
{
  QMutexLocker l( &mMutex );
  mVerbosity = verbosity;
  setErrorCode( ssh_options_set(mSession.get(),
                                SSH_OPTIONS_LOG_VERBOSITY,
                                &mVerbosity ) );
  Q_UNUSED( l );
}

void SshTools::tryConnect
(
    const QString           & remouteHost,
    const unsigned          & remoutePort,
    int                     verbosity
)
{
  tryDisconnect();

  {
    QMutexLocker l( &mMutex );
    mSession.reset( ssh_new() );
    Q_UNUSED( l );
  }

  setHost     ( remouteHost );
  setVerbosity( verbosity );
  setPort     ( remoutePort );
//  setUser     ( remouteUser );
  mConnected  = true;

  setErrorCode( ssh_connect( mSession.get() ) );
}

void                    SshTools::setPassword(const QString& pwd)
{
  if( mPassword != pwd )
  {
    mPassword = pwd;
  }
}

int SshTools::InteractiveShell()
{
  int rc;
//  char buffer[256];
//  int nbytes;
  rc = ssh_channel_request_pty(mChannel.get());
  if (rc != SSH_OK) return rc;
  rc = ssh_channel_change_pty_size(mChannel.get(), 80, 24);
  if (rc != SSH_OK) return rc;
  rc = ssh_channel_request_shell(mChannel.get());
  if (rc != SSH_OK) return rc;

  char buffer[256];
  int nbytes, nwritten;
  while ( isReady() and
          ssh_channel_is_open(mChannel.get()) and
         (!ssh_channel_is_eof(mChannel.get())))
  {
    struct timeval timeout;
    ssh_channel in_channels[2], out_channels[2];
    fd_set fds;
    int maxfd;
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;
    in_channels[0] = mChannel.get();
    in_channels[1] = NULL;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    FD_SET(ssh_get_fd(mSession.get()), &fds);
    maxfd = ssh_get_fd(mSession.get()) + 1;
    ssh_select(in_channels, out_channels, maxfd, &fds, &timeout);
    if (out_channels[0] != NULL)
    {
      nbytes = ssh_channel_read(mChannel.get(), buffer, sizeof(buffer), 0);
      if (nbytes < 0) return SSH_ERROR;
      if (nbytes > 0)
      {
        nwritten = write(1, buffer, nbytes);
        if (nwritten != nbytes)
          return SSH_ERROR;
      }
    }
    if (FD_ISSET(0, &fds))
    {
      nbytes = read(0, buffer, sizeof(buffer));
      if (nbytes < 0) return SSH_ERROR;
      if (nbytes > 0)
      {
        nwritten = ssh_channel_write(mChannel.get(), buffer, nbytes);
        if (nbytes != nwritten)
          return SSH_ERROR;
      }
    }
  }
  return rc;
}

}// namespace           net
}// namespace           core
}// namespace           sospo
