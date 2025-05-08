#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "mqttclient.h"

// BME280传感器数据结构
struct bme280_data {
    int temp;
    unsigned int press;
    unsigned int hum;
};

// SGP30传感器数据结构
struct sgp30_data {
    uint16_t co2;
    uint16_t tvoc;
};

static int bme280_fd = -1;
static int sgp30_fd = -1;

// 初始化BME280传感器
static int bme280_init() {
    bme280_fd = open("/dev/bme280", O_RDONLY);
    if (bme280_fd < 0) {
        perror("BME280打开失败");
        return -1;
    }
    return 0;
}

// 初始化SGP30传感器
static int sgp30_init() {
    sgp30_fd = open("/dev/sgp30", O_RDONLY);
    if (sgp30_fd < 0) {
        perror("SGP30打开失败");
        return -1;
    }
    return 0;
}

// 读取BME280数据
static int bme280_read_data(struct bme280_data *data) {
    ssize_t ret = read(bme280_fd, data, sizeof(*data));
    if (ret != sizeof(*data)) {
        if (errno == EAGAIN) return -1;
        perror("BME280读取失败");
        return -2;
    }
    return 0;
}

// 读取SGP30数据
static int sgp30_read_data(struct sgp30_data *data) {
    char buffer[64];
    ssize_t ret = read(sgp30_fd, buffer, sizeof(buffer));
    
    if (ret > 0) {
        // 解析数据格式："CO2_value TVOC_value"
        if (sscanf(buffer, "%hu %hu", &data->co2, &data->tvoc) != 2) {
            perror("SGP30数据解析失败");
            return -1;
        }
        return 0;
    } else {
        if (errno == EAGAIN) return -1;
        perror("SGP30读取失败");
        return -2;
    }
}

// 上报数据到阿里云
static void report_sensor_data(mqtt_client_t *client, 
                             float temp, float hum, float press,
                             uint16_t co2, uint16_t tvoc) {
    char payload[512];
    snprintf(payload, sizeof(payload),
        "{\"method\":\"thing.event.property.post\","
        "\"id\":\"%ld\","
        "\"params\":{"
        "\"znzc:temperature\":%.1f,"
        "\"znzc:Humidity\":%.1f,"
        "\"znzc:Atmosphere\":%.2f,"
        "\"znzc:CO2\":%hu,"
        "\"znzc:TVOC\":%hu"
        "},"
        "\"version\":\"1.0\"}",
        (long)time(NULL), temp, hum, press, co2, tvoc);

    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.payload = payload;
    msg.qos = 0;
    
    mqtt_publish(client, "/sys/grv75dUlz7w/TestDev/thing/event/property/post", &msg);
}

void *mqtt_publish_thread(void *arg) {
    mqtt_client_t *client = (mqtt_client_t *)arg;
    struct bme280_data bme_data;
    struct sgp30_data sgp_data;
    
    // 初始化传感器
    if (bme280_init() != 0 || sgp30_init() != 0) {
        MQTT_LOG_E("Sensor init failed");
        return NULL;
    }

    while (1) {
        // 读取BME280数据
        if (bme280_read_data(&bme_data) == 0) {
            // 读取SGP30数据
            if (sgp30_read_data(&sgp_data) == 0) {
                // 数据转换
                float temp = bme_data.temp / 100.0f;
                float hum = bme_data.hum / 1024.0f;
                float press = bme_data.press / 25600.0f;
                
                // 上报数据
                report_sensor_data(client, temp, hum, press, 
                                 sgp_data.co2, sgp_data.tvoc);
                
                MQTT_LOG_I("Reported: Temp=%.1fC, Hum=%.1f%%, Press=%.2fhPa, CO2=%huppm, TVOC=%huppb", 
                          temp, hum, press, sgp_data.co2, sgp_data.tvoc);
            }
        }
        sleep(1); // 每秒上报一次
    }
    return NULL;
}

int main(void) {
    int res;
    pthread_t thread1;
    mqtt_client_t *client = NULL;
    
    printf("\nMQTT Sensor Data Reporter\n");

    mqtt_log_init();

    client = mqtt_lease();
    
    // 配置MQTT连接参数
    mqtt_set_port(client, "1883");
    mqtt_set_host(client, "iot-06z00gj9pozogc5.mqtt.iothub.aliyuncs.com");
    mqtt_set_client_id(client, "grv75dUlz7w.TestDev|securemode=2,signmethod=hmacsha256,timestamp=1745064195182|");
    mqtt_set_user_name(client, "TestDev&grv75dUlz7w");
    mqtt_set_password(client, "48a9abf7ab5109f056d1704f6e2037895eb76db82b90888042e8a27c033bda67");
    mqtt_set_clean_session(client, 1);

    // 连接MQTT服务器
    mqtt_connect(client);
    
    // 创建数据上报线程
    res = pthread_create(&thread1, NULL, mqtt_publish_thread, client);
    if(res != 0) {
        MQTT_LOG_E("create mqtt publish thread fail");
        exit(res);
    }

    while (1) {
        sleep(100);
    }
}