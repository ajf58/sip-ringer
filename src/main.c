/*
 * Copyright (C) 2015 Andrew Featherstone <andrew.featherstone@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <pjsua.h>

#define SIP_DOMAIN "192.168.56.1"
#define SIP_REALM "asterisk"
#define SIP_USER "202b"
#define SIP_PASSWD "202b"

/*============================================================================*
 * Private function declarations
 *============================================================================*/
static void initialize_pjsua(void);
static void add_transport(void);
static void register_account(void);
static void error_exit(const char *title, pj_status_t status);
static void on_call_state(pjsua_call_id call_id, pjsip_event *e);
static void on_incoming_call(
    pjsua_acc_id acc_id, 
    pjsua_call_id call_id,
    pjsip_rx_data *rdata
    );

/*============================================================================*
 * Private variables 
 *============================================================================*/
static pjsua_acc_id account_id = PJSUA_INVALID_ID;
static pjsua_transport_id trans_id = PJSUA_INVALID_ID;

/*============================================================================*
 * Public function definitions
 *============================================================================*/
/**
 * Application entry point.
 */
int main(int argc, char **argv)
{
    int res = EXIT_FAILURE;
    if (pjsua_create() == PJ_SUCCESS)
    {
        initialize_pjsua();
        add_transport();
        register_account();

        /* Initialization is done, now start pjsua */
        pj_status_t status = pjsua_start();
        if (status != PJ_SUCCESS)
        {
            error_exit("Error starting pjsua", status);
        }
    
        pjsua_acc_id acc_id = 0;


        while (1)
        {
            /* Spin */
        }
        
        pjsua_destroy();
    }

    return res;
}

/*============================================================================*
 * Private function definitions
 *============================================================================*/

/**
 * Application-wide initialization of the PJSUA library.
 */
static void initialize_pjsua(void)
{
    pjsua_config ua_cfg;
    pjsua_logging_config log_cfg;
    pjsua_media_config media_cfg;

    // Initialize configs with default settings.
    pjsua_config_default(&ua_cfg);
    pjsua_logging_config_default(&log_cfg);
    pjsua_media_config_default(&media_cfg);

    // At the very least, application would want to override
    // the call callbacks in pjsua_config:
    ua_cfg.cb.on_incoming_call = on_incoming_call;
    ua_cfg.cb.on_call_state = on_call_state;

    /* The default logging level (5) is very verbose, so dial it down. */
    log_cfg.console_level = 4;
    // Customize other settings (or initialize them from application specific
    // configuration file):
    // Initialize pjsua
    pj_status_t status = pjsua_init(&ua_cfg, &log_cfg, &media_cfg);
    if (status != PJ_SUCCESS)
    {
        pjsua_perror(__FILE__, "Error initializing pjsua", status);
    }
}

/**
 * Add UDP transport.
 */
static void add_transport(void)
{
    pjsua_transport_config cfg;

    pjsua_transport_config_default(&cfg);
    cfg.port = 5060;
    pj_status_t status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, &trans_id);
    if (status != PJ_SUCCESS)
    {
        error_exit("Error creating transport", status);
    }
}

/**
 * Register to SIP server by creating SIP account.
 */
static void register_account(void)
{
    pjsua_acc_config cfg;

    pjsua_acc_config_default(&cfg);
    cfg.id = pj_str("sip:" SIP_USER "@" SIP_DOMAIN);
    cfg.reg_uri = pj_str("sip:" SIP_DOMAIN);
    cfg.cred_count = 1;
    cfg.cred_info[0].realm = pj_str(SIP_REALM);
    cfg.cred_info[0].scheme = pj_str("digest");
    cfg.cred_info[0].username = pj_str(SIP_USER);
    cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    cfg.cred_info[0].data = pj_str(SIP_PASSWD);

    pj_status_t status = pjsua_acc_add(&cfg, PJ_TRUE, &account_id);
    if (status != PJ_SUCCESS)
    {
        error_exit("Error adding account", status);
    }
}

static void error_exit(const char *title, pj_status_t status)
{
    pjsua_perror(__FILE__, title, status);
    pjsua_destroy();
    exit(EXIT_FAILURE);
}
/**
 * Callback called by the library when call's state has changed 
 */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(e);

    pjsua_call_get_info(call_id, &ci);
    PJ_LOG(3,(__FILE__, "Call %d state=%.*s", call_id,
                (int)ci.state_text.slen,
                ci.state_text.ptr));
}

/**
 * Callback function for handling incoming calls.
 */
static void on_incoming_call(
    pjsua_acc_id acc_id, 
    pjsua_call_id call_id,
    pjsip_rx_data *rdata
    )
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);

    pjsua_call_get_info(call_id, &ci);

    PJ_LOG(3,(__FILE__, "Incoming call from %.*s!!",
                (int)ci.remote_info.slen,
                ci.remote_info.ptr));

    /* Automatically answer incoming calls with 200/OK */
  //  pjsua_call_answer(call_id, 200, NULL, NULL);
}
