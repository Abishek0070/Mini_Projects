#include <ESP32-CAM.h>
#include <HCSR04.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>

// --- 1. WiFi Credentials ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// --- 2. Blynk Authentication Token ---
char auth[] = "YOUR_BLYNK_AUTH_TOKEN";

// --- 3. Ultrasonic Sensor Pins (Changed to avoid conflict) ---
#define TRIGGER_PIN 12
#define ECHO_PIN 14

// --- 4. Obstacle Distance Threshold (in cm) ---
#define OBSTACLE_THRESHOLD 20

// --- 5. Blynk Virtual Pins ---
#define DISTANCE_VPIN V0
#define OBSTACLE_LED_VPIN V1

// --- 6. Camera Configuration (Check your ESP32-CAM variant) ---
camera_config_t camera_config = {
  .pin_pwdn = 32,        // Power Down
  .pin_reset = -1,       // Reset
  .pin_xclk = 0,         // External Clock
  .pin_sccb_sda = 13,    // SCCB Data
  .pin_sccb_scl = 15,    // SCCB Clock
  .pin_pclk = 25,        // Pixel Clock
  .pin_vsync = 27,       // Vertical Sync
  .pin_href = 26,        // Horizontal Reference
  .pin_data0 = 21,       // Data 0
  .pin_data1 = 19,       // Data 1
  .pin_data2 = 18,       // Data 2
  .pin_data3 = 5,        // Data 3
  .pin_data4 = 17,       // Data 4
  .pin_data5 = 16,       // Data 5
  .pin_data6 = 4,        // Data 6
  .pin_data7 = 34,       // Data 7
  .xclk_freq_hz = 20000000
};

// --- 7. Global Objects ---
HCSR04 ultrasonic(TRIGGER_PIN, ECHO_PIN);
BlynkTimer timer;

void setup() {
  Serial.begin(115200);

  // --- 8. Initialize WiFi and Blynk ---
  Blynk.begin(auth, ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // --- 9. Initialize Camera ---
  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }

  // --- 10. Set Camera Resolution ---
  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA); // 320x240

  // --- 11. Setup obstacle check timer ---
  timer.setInterval(1000L, checkObstacles); // every 1 second
}

void loop() {
  Blynk.run();
  timer.run();
}

void checkObstacles() {
  float distance = ultrasonic.read();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Send distance to Blynk app
  Blynk.virtualWrite(DISTANCE_VPIN, distance);

  if (distance < OBSTACLE_THRESHOLD) {
    Serial.println("Obstacle detected!");
    Blynk.notify("Obstacle detected!");
    Blynk.virtualWrite(OBSTACLE_LED_VPIN, 255); // turn on virtual LED
  } else {
    Blynk.virtualWrite(OBSTACLE_LED_VPIN, 0);   // turn off virtual LED
  }
}
