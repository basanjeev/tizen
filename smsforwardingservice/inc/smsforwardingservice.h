#ifndef __smsforwardingservice_H__
#define __smsforwardingservice_H__

#include <dlog.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "smsforwardingservice"

#include <messages.h>
#include <email.h>
#include <message_port.h>


typedef struct {
    messages_service_h service_handle;
    messages_message_h msg;
    email_h emsg;
} appdata_s;

#define _PRINT_MSG_LOG_BUFFER_SIZE_ 1024
#define PRINT_MSG(fmt, args...) do { char _log_[_PRINT_MSG_LOG_BUFFER_SIZE_]; \
    snprintf(_log_, _PRINT_MSG_LOG_BUFFER_SIZE_, fmt, ##args);} while (0)



#endif /* __smsforwardingservice_H__ */
