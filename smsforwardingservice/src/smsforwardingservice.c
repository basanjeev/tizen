#include <tizen.h>
#include <service_app.h>
#include "smsforwardingservice.h"

char recipient[1024] = "iamsanjeev@gmail.com";

bool check_error(int return_value, int success_code, char *msg)
{
    if (return_value != success_code) {
        dlog_print(DLOG_ERROR, LOG_TAG, "%s. Error: %s [code: %d]",msg, get_error_message(return_value), return_value);
        return true;
    }
    return false;
}

void new_msg_cb(messages_message_h incoming_msg, void *user_data){

	int error_code = 0;
	appdata_s *ad = (appdata_s *)user_data;
	/* Remove recipients */

	if(ad->msg)
	{
		error_code = email_remove_all_recipients(ad->emsg);
		check_error(error_code, EMAILS_ERROR_NONE, "Failed to remove recipients");

		/* Remove attachments */
		error_code = email_remove_all_attachments(ad->emsg);
		check_error(error_code, EMAILS_ERROR_NONE, "Failed to remove attachments");

		/* Remove message sending status callback */
		error_code = email_unset_message_sent_cb(ad->emsg);
		check_error(error_code, EMAILS_ERROR_NONE, "Failed to unset status callback");

		/* Destroy the message */
		error_code = email_destroy_message(ad->emsg);
		check_error(error_code, EMAILS_ERROR_NONE, "Failed to destroy email message");
	}

	ad->emsg = NULL;

    error_code = email_create_message(&ad->emsg);
    if (check_error(error_code, EMAILS_ERROR_NONE, "Failed to create email message"))
        return;

    error_code = email_set_subject(ad->emsg, "New SMS From India");
    check_error(error_code, EMAILS_ERROR_NONE, "Failed to set subject");


    char *body = NULL;
    messages_get_text (incoming_msg, &body);
    error_code = email_set_body(ad->emsg, body);
    check_error(error_code, EMAILS_ERROR_NONE, "Failed to set body");
    free(body);


    error_code = email_add_recipient(ad->emsg, EMAIL_RECIPIENT_TYPE_TO, recipient);
    check_error(error_code, EMAILS_ERROR_NONE, "Failed to add recipient");

    /* Save the email */
    error_code = email_save_message(ad->emsg);
    check_error(error_code, EMAILS_ERROR_NONE, "Failed to save email");

    /* Set the message sending status callback */
    /* Send message */
    error_code = email_send_message(ad->emsg, false);
    if (!check_error(error_code, EMAILS_ERROR_NONE, "Failed to send email"))
        PRINT_MSG("Sending email, please wait for message printed by status callback");

}
void message_port_cb(int local_port_id, const char *remote_app_id, const char *remote_port,
                     bool trusted_remote_port, bundle *message, void *user_data)
{
    PRINT_MSG("message_port_cb");
    char *command = NULL;
    char *data = NULL;
    bundle_get_str(message, "command", &command);
    bundle_get_str(message, "data", &data);

    PRINT_MSG("Message from %s:", remote_app_id);
    PRINT_MSG("Command: %s", command);
    PRINT_MSG("Data: %s", data);
    dlog_print(DLOG_INFO, LOG_TAG, "Msg %s, command: %s, data: %s", remote_app_id, command, data);
    if(data!=NULL && strlen(data)>5){
    	strncpy(recipient, data, 1024);
    }
}


bool service_app_create(void *data)
{
	appdata_s *ad = (appdata_s *) data;
    int error_code = messages_open_service(&ad->service_handle);
    if (error_code != MESSAGES_ERROR_NONE) {
        dlog_print(DLOG_ERROR, LOG_TAG, "Failed to open messages service");
        return false;
    }
    error_code = messages_set_message_incoming_cb(ad->service_handle, new_msg_cb, data);
    if (error_code != MESSAGES_ERROR_NONE) {
        dlog_print(DLOG_ERROR, LOG_TAG, "Failed to set callback");
        return false;
    }
    int local_port_id = message_port_register_local_port("smsforwarderservice", message_port_cb, data);
    if (local_port_id < 0)
    {
       dlog_print(DLOG_ERROR, LOG_TAG, "Port register error : %d", local_port_id);
       return false;
    }
    else
    {
       dlog_print(DLOG_INFO, LOG_TAG, "port_id : %d", local_port_id);
    }
    return true;
}

void service_app_terminate(void *data)
{
	appdata_s *ad = (appdata_s *)data;
    int error_code = messages_close_service(ad->service_handle);
    if (error_code != MESSAGES_ERROR_NONE)
        dlog_print(DLOG_ERROR, LOG_TAG, "Failed to close messages service, error: %d", error_code);
    return;
}

void service_app_control(app_control_h app_control, void *data)
{
    // Todo: add your code here.
    return;
}

int main(int argc, char* argv[])
{
    appdata_s ad;
    memset(&ad, 0x00, sizeof(appdata_s));
    service_app_lifecycle_callback_s event_callback;
	event_callback.create = service_app_create;
	event_callback.terminate = service_app_terminate;
	event_callback.app_control = service_app_control;

	return service_app_main(argc, argv, &event_callback, &ad);
}
