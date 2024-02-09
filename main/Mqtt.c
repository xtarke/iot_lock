/* Copyright (c) 2023 Renan Augusto Starke
 *
 * This file is part of project "IoT Lock".
 * 
 */

/**
 * @file Mqtt.cp
 * @author Renan Augusto Starke
 * @date 14 DEc 2023
 * @brief File containing Mqtt C functions. Based on MQQT5 SDK example.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "Mqtt.h"

static const char *TAG = "MQTT5";

/* Queue to stored received tasg */
static QueueHandle_t subscribe_queue;

/* Publish mutex */
static SemaphoreHandle_t xSemaphore_publish;

/* Mqtt client information */
static esp_mqtt_client_handle_t client;

/**
 * @brief Get the tag received from MQTT. Blocks until a new tag is received.
 * 
 * @return uint32_t Tag number.
 */
uint32_t get_added_tag(){

	uint32_t tag = -1;

	if( !xQueueReceive( subscribe_queue, &(tag), portMAX_DELAY / portTICK_PERIOD_MS) )
	{
		return -1;
	}

	return tag;
}


void mqtt5_publish(const char *topic, char *msg){
	int msg_id;

	xSemaphoreTake(xSemaphore_publish, portMAX_DELAY);
	msg_id = esp_mqtt_client_publish(client, topic, msg, 0, 0, 0);
	xSemaphoreGive(xSemaphore_publish);
}


static void log_error_if_nonzero(const char *message, int error_code)
{
	if (error_code != 0) {
		ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
	}
}

static esp_mqtt5_user_property_item_t user_property_arr[] = {
		{"board", "esp32"},
		{"u", "user"},
		{"p", "password"}
};

#define USE_PROPERTY_ARR_SIZE   sizeof(user_property_arr)/sizeof(esp_mqtt5_user_property_item_t)

static esp_mqtt5_publish_property_config_t publish_property = {
		.payload_format_indicator = 1,
		.message_expiry_interval = 1000,
		.topic_alias = 0,
		.response_topic = "/topic/test/response",
		.correlation_data = "123456",
		.correlation_data_len = 6,
};

static esp_mqtt5_subscribe_property_config_t subscribe_property = {
		.subscribe_id = 25555,
		.no_local_flag = false,
		.retain_as_published_flag = false,
		.retain_handle = 0,
		.is_share_subscribe = true,
		.share_name = "group1",
};

static esp_mqtt5_subscribe_property_config_t subscribe1_property = {
		.subscribe_id = 25555,
		.no_local_flag = true,
		.retain_as_published_flag = false,
		.retain_handle = 0,
};

static esp_mqtt5_unsubscribe_property_config_t unsubscribe_property = {
		.is_share_subscribe = true,
		.share_name = "group1",
};

static esp_mqtt5_disconnect_property_config_t disconnect_property = {
		.session_expiry_interval = 60,
		.disconnect_reason = 0,
};

static void print_user_property(mqtt5_user_property_handle_t user_property)
{
	if (user_property) {
		uint8_t count = esp_mqtt5_client_get_user_property_count(user_property);
		if (count) {
			esp_mqtt5_user_property_item_t *item = (esp_mqtt5_user_property_item_t *)malloc(count * sizeof(esp_mqtt5_user_property_item_t));
			if (esp_mqtt5_client_get_user_property(user_property, item, &count) == ESP_OK) {
				for (int i = 0; i < count; i ++) {
					esp_mqtt5_user_property_item_t *t = &item[i];
					ESP_LOGI(TAG, "key is %s, value is %s", t->key, t->value);
					free((char *)t->key);
					free((char *)t->value);
				}
			}
			free(item);
		}
	}
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt5_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
	esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
	esp_mqtt_client_handle_t client = event->client;
	int msg_id;

	ESP_LOGD(TAG, "free heap size is %" PRIu32 ", minimum %" PRIu32, esp_get_free_heap_size(), esp_get_minimum_free_heap_size());
	switch ((esp_mqtt_event_id_t)event_id) {
	case MQTT_EVENT_CONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

		print_user_property(event->property->user_property);
		esp_mqtt5_client_set_user_property(&publish_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
		esp_mqtt5_client_set_publish_property(client, &publish_property);

			/* Subscribe to broker topic to add or remove permissible tags */
		esp_mqtt5_client_set_user_property(&subscribe_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
		esp_mqtt5_client_set_subscribe_property(client, &subscribe_property);
		msg_id = esp_mqtt_client_subscribe(client, "/lpae/add_tag", 0);
		esp_mqtt5_client_delete_user_property(subscribe_property.user_property);
		subscribe_property.user_property = NULL;
		ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
		break;




	case MQTT_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
		print_user_property(event->property->user_property);
		break;
	case MQTT_EVENT_SUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
		//print_user_property(event->property->user_property);
		//esp_mqtt5_client_set_publish_property(client, &publish_property);
		//msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
		//ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
		break;

	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		print_user_property(event->property->user_property);
		esp_mqtt5_client_set_user_property(&disconnect_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
		esp_mqtt5_client_set_disconnect_property(client, &disconnect_property);
		esp_mqtt5_client_delete_user_property(disconnect_property.user_property);
		disconnect_property.user_property = NULL;
		esp_mqtt_client_disconnect(client);
		break;


	case MQTT_EVENT_PUBLISHED:
		ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
		print_user_property(event->property->user_property);
		break;

	case MQTT_EVENT_DATA:
		//ESP_LOGI(TAG, "MQTT_EVENT_DATA");
		//print_user_property(event->property->user_property);
		//ESP_LOGI(TAG, "payload_format_indicator is %d", event->property->payload_format_indicator);
		//ESP_LOGI(TAG, "response_topic is %.*s", event->property->response_topic_len, event->property->response_topic);
		//ESP_LOGI(TAG, "correlation_data is %.*s", event->property->correlation_data_len, event->property->correlation_data);
		//ESP_LOGI(TAG, "content_type is %.*s", event->property->content_type_len, event->property->content_type);
		//ESP_LOGI(TAG, "TOPIC=%.*s", event->topic_len, event->topic);
		//ESP_LOGI(TAG, "DATA=%.*s", event->data_len, event->data);

		/* Convert a tag to int and enqueue it  */
		uint32_t tag = atoi(event->data);
		xQueueSend( subscribe_queue, (void *) &tag, ( TickType_t ) 0 );
		/* Reset string buffer to avoid string overlapping */
		memset(event->data, 0, event->data_len);

		break;

	case MQTT_EVENT_ERROR:
		ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
		print_user_property(event->property->user_property);
		ESP_LOGI(TAG, "MQTT5 return code is %d", event->error_handle->connect_return_code);
		if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
			log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
			log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
			log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
			ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
		}
		break;
	default:
		ESP_LOGI(TAG, "Other event id:%d", event->event_id);
		break;
	}
}

/**
 * @brief Start MQQT5 protocol.
 * 
 */
static void mqtt5_app_start(void)
{
	esp_mqtt5_connection_property_config_t connect_property = {
			.session_expiry_interval = 10,
			.maximum_packet_size = 1024,
			.receive_maximum = 65535,
			.topic_alias_maximum = 2,
			.request_resp_info = true,
			.request_problem_info = true,
			.will_delay_interval = 10,
			.message_expiry_interval = 10,
			.payload_format_indicator = true,
			.response_topic = "/test/response",
			.correlation_data = "123456",
			.correlation_data_len = 6,
	};

	esp_mqtt_client_config_t mqtt5_cfg = {
			.broker.address.uri = CONFIG_BROKER_URL,
			.broker.address.port = 8883,
			.session.protocol_ver = MQTT_PROTOCOL_V_5,
			.network.disable_auto_reconnect = true,
			.credentials.username = CONFIG_BROKER_USER,
			.credentials.authentication.password = CONFIG_BROKER_PASSWORD,
			.session.last_will.topic = "/topic/will",
			.session.last_will.msg = "i will leave",
			.session.last_will.msg_len = 12,
			.session.last_will.qos = 1,
			.session.last_will.retain = true,
	};

	client = esp_mqtt_client_init(&mqtt5_cfg);

	/* Set connection properties and user properties */
	esp_mqtt5_client_set_user_property(&connect_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
	esp_mqtt5_client_set_user_property(&connect_property.will_user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
	esp_mqtt5_client_set_connect_property(client, &connect_property);

	/* If you call esp_mqtt5_client_set_user_property to set user properties, DO NOT forget to delete them.
	 * esp_mqtt5_client_set_connect_property will malloc buffer to store the user_property and you can delete it after
	 */
	esp_mqtt5_client_delete_user_property(connect_property.user_property);
	esp_mqtt5_client_delete_user_property(connect_property.will_user_property);

	/* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
	esp_mqtt_client_register_event(client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt5_event_handler, NULL);
	esp_mqtt_client_start(client);
}

/**
 * @brief Initialize MQTT5 Protocol. Broker setting come from SDK config.
 * 
 */
void mqtt5_init(void)
{

	ESP_LOGI(TAG, "[APP] Startup..");
	ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
	ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

	esp_log_level_set("*", ESP_LOG_INFO);
	esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
	esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
	esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
	esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
	esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
	esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

	/* Create a Queue to store received tag number */
	subscribe_queue = xQueueCreate(10, sizeof( uint32_t ));

	xSemaphore_publish = xSemaphoreCreateMutex();

	//ESP_ERROR_CHECK(nvs_flash_init());
	//ESP_ERROR_CHECK(esp_netif_init());
	//ESP_ERROR_CHECK(esp_event_loop_create_default());

	/* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
	 * Read "Establishing Wi-Fi or Ethernet Connection" section in
	 * examples/protocols/README.md for more information about this function.
	 */
	//ESP_ERROR_CHECK(example_connect());

	mqtt5_app_start();
}
