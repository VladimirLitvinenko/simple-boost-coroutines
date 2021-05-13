#ifndef DOCUMENTLITERALS_H
#define DOCUMENTLITERALS_H

namespace                       spo   {
namespace                       core  {
namespace                       docs  {

const int DOC_UNDEFINED_ID      = -1;

const int DOC_VALUE_ID          = 1;

const int DOC_DEVICE_ID         = 10;
const int DOC_DEVICES_ID        = DOC_DEVICE_ID + 1;
const int DOC_NETIFACE_ID       = DOC_DEVICE_ID + 2;
const int DOC_NETIFACES_ID      = DOC_DEVICE_ID + 3;
const int DOC_NETECHO_ID        = DOC_DEVICE_ID + 4;

const int DOC_USER_ID           = 20;
const int DOC_USERS_ID          = DOC_USER_ID + 1;
const int DOC_GROUP_ID          = DOC_USERS_ID + 1;
const int DOC_GROUPS_ID         = DOC_GROUP_ID + 1;

const int DOC_DOCPKG_ID          = 30;

const int DOC_BYTEARRAY_ID      = 40;
const int DOC_BYTESTUFFING_ID   = 50;

const int DOC_ARM_ID            = 100;
const int DOC_ARMS_ID           = DOC_ARM_ID + 1;
const int DOC_ARMSETTINGS_ID    = DOC_ARM_ID + 2;

const int DOC_IVS_DOMAIN_ID     = 200;

const int DOC_IVS_DOCUMENT_ID   = 300;
const int DOC_IVS_ROLES_ID      = DOC_IVS_DOCUMENT_ID + 1;
const int DOC_ARM_SETTINGS_ID   = DOC_IVS_DOCUMENT_ID + 2;
const int DOC_ARMS_SETTINGS_ID  = DOC_IVS_DOCUMENT_ID + 3;
const int DOC_ARM_NET_IFACE_ID  = DOC_IVS_DOCUMENT_ID + 4;
const int DOC_ARM_NET_IFACES_ID = DOC_IVS_DOCUMENT_ID + 5;
const int DOC_ARM_SYSROLE_ID    = DOC_IVS_DOCUMENT_ID + 6;
const int DOC_ARM_LOCALROLE_ID  = DOC_IVS_DOCUMENT_ID + 7;
const int DOC_ARM_NAME          = DOC_IVS_DOCUMENT_ID + 8;
const int DOC_ARM_ACTIVE        = DOC_IVS_DOCUMENT_ID + 9;
const int DOC_ARM_MASTER        = DOC_IVS_DOCUMENT_ID + 10;
const int DOC_ARM_WEIGHT        = DOC_IVS_DOCUMENT_ID + 11;
const int DOC_ARM_IP            = DOC_IVS_DOCUMENT_ID + 12;

//const int DOC_ASIO_BUFFER       = 400;

const int DOC_ICMP_ID           = 400;
const int DOC_IP4_ID            = DOC_ICMP_ID + 10;
const int DOC_PING_INFO_ID      = DOC_ICMP_ID + 20;
const int DOC_PING_CTRL_ID      = DOC_ICMP_ID + 21;

}// namespace                   docs
}// namespace                   core
}// namespace                   spo

#endif // DOCUMENTLITERALS_H
