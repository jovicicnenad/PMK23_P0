/*
 * Program implements MQTT client on a NUCLEO-L476RG board
 * using arm mbed-mqtt library and ESP-WROOM-02 WiFi modem.
 *
 * University of Belgrade - School of Electrical Engineering
 * Department of Electronics
 * Bulevar Kralja Aleksandra 73, 11120 Belgrade, Serbia
 *
 * March 2023.
 *
 */

#define MBED_CONF_TARGET_DEFAULT_ADC_VREF 0
#include "mbed.h"
#include "stm_mb_pins.h"
#include "platform/mbed_thread.h"
#include "MQTTClientMbedOs.h"


// LED2 blinking rate:
#define BLINKING_RATE_MS                                                     250
// Scaler to 3v3L
#define VOLTAGE_SCALER                                                      3.3f
// Client yield timeout in miliseconds:
#define YIELD_TIMEOUT_MS                                                    1000
// Maximum number of networks to scan for:
#define MAX_NETWORKS                                                          15
// Small delay for network information printing:
#define PRINTF_DELAY_MS                                                       10


// Left potentiometer:
AnalogIn pot1(MB_POT1);
// Left button on the motherboard:
InterruptIn sw1(MB_SW1);
// Right LED on the motherboard:
DigitalOut led2(MB_LED2);

NetworkInterface *net;
// Pointer to a WiFi network object:
WiFiInterface *wifi;
// Creating TCP socket:
TCPSocket socket;
// Creating MQTT client using the TCP socket;
MQTTClient client(&socket);
// Message handler:
MQTT::Message message;

//publish topic
char* topic = "mbed-sample-pub";
//subscribe topic
char* topic_sub = "mbed-sample-sub";
// Counter of arrived messages:
int arrivedcount = 0;
// Flag indicating that button is not pressed:
int button_pressed=0;
// HiveMQ broker connectivity information:
const char* hostname = "broker.hivemq.com";
//
int mqtt_port = 1883;
// Returning a string for a provided network encryption: 
const char *sec2str(nsapi_security_t sec)
{
    switch (sec) 
    {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

int scan_networks(WiFiInterface *wifi)
{
    printf("Scan:\n");
    
    // Scan only for the number of networks, first parameter is NULL:
    int count = wifi->scan(NULL, 0);
    // If there are no networks, count == 0, if there is an error, counter < 0:
    if (count <= 0)
    {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    // Limit number of network arbitrary to some reasonable number:
    count = count < MAX_NETWORKS ? count : MAX_NETWORKS;
    
    // Create a local pointer to an object, which is an array of WiFi APs:
    WiFiAccessPoint *ap = new WiFiAccessPoint[count];
    // Now scan again for 'count' networks and populate the array of APs:
    count = wifi->scan(ap, count);
    
    // This time, the number of entries to 'ap' is returned:
    if (count <= 0) 
    {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }
    
    // Print out the parameters of each AP:
    for (int i = 0; i < count; i++) 
    {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
        thread_sleep_for(PRINTF_DELAY_MS);
    }
    printf("%d networks available.\n", count);
    
    // Since 'ap' is dynamically allocated pointer to the array of objects, it
    // needs to be deleted:
    delete[] ap;
    return count;
}

void print_network_info()
    {
        /* print the network info */
        SocketAddress a;
        net->get_ip_address(&a);
        printf("IP address: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        net->get_netmask(&a);
        printf("Netmask: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        net->get_gateway(&a);
        printf("Gateway: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    }

void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    //printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\r\n", message.qos, message.retained, message.dup, message.id);
    printf("Message from the browser: %.*s\r\n", message.payloadlen, (char*)message.payload);
    ++arrivedcount;
}

void buttonFunction() {
    
    button_pressed=1;
   
}

int main()
{
    // Set the interrupt event:
    sw1.fall(&buttonFunction); 

    net = NetworkInterface::get_default_instance();

    wifi = net->wifiInterface();

    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }
    
    // Scan for available networks and aquire information about Access Points:
    int count = scan_networks(wifi);
    if (count == 0) {
        printf("No WIFI APs found - can't continue further.\n");
        return -1;
    }
    
    nsapi_size_or_error_t result = net->connect();
    if (result != 0) {
        printf("Error! _net->connect() returned: %d\r\n", result);
    }
    
    print_network_info();

    // Open TCP socket using WiFi network interface:
    socket.open(net);

    SocketAddress address;

    //const char hostname[] = "broker.hivemq.com";

    result = net->gethostbyname(hostname, &address);
    if (result != 0) {
        printf("Error! gethostbyname(%s) returned: %d\r\n", hostname, result);
        return false;
    }

    address.set_port(mqtt_port);

    result = socket.connect(address);
    if (result != 0) {
        printf("Error! socket.connect() returned: %d\r\n", result);
    }

    // Fill connect data with default values:
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    // Change only ID and protocol version:
    data.MQTTVersion = 3;
    data.clientID.cstring = "NUCLEO-L476RG-40";
    
    // Connect the MQTT client 
    int rc = 0;
    if ((rc = client.connect(data)) != 0)
        printf("rc from MQTT connect is %d\r\n", rc);

    //Sunscribe to the topic
    if ((rc = client.subscribe(topic_sub, MQTT::QOS2, messageArrived)) != 0)
        printf("rc from MQTT subscribe is %d\r\n", rc);
      
    while (true) {
        // Show that the loop is running by switching motherboard LED2:
        led2 = !led2;
        thread_sleep_for(BLINKING_RATE_MS);
        if (button_pressed==1) {
            button_pressed=0;      
            // QoS 0
            char buf[100];
            sprintf(buf, "V(POT1) = %.2f\r\n", pot1*VOLTAGE_SCALER);
            message.qos = MQTT::QOS0;
            message.retained = false;
            message.dup = false;
            message.payload = (void*)buf;
            message.payloadlen = strlen(buf)+1;
            int retval=client.publish(topic, message);
            printf("%d, %s\n", retval, buf);
        }
        // Need to call yield API to maintain connection:
        client.yield(YIELD_TIMEOUT_MS);
    }
}