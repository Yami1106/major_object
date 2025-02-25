#include <WiFi.h>
#include <HTTPClient.h>

// WiFi credentials
const char* ssid = "S23";
const char* password = "viropo2310";

// Google API Key (Keep this private!)
const char* apiKey = "AIzaSyDrlMN-Sg61sv1TJYqSKIT-VDLkV2PfH3A"; // Replace with a valid API key

// Function to scan WiFi networks and send data to Google API
void getLocation() {
    Serial.println("🔎 Scanning WiFi networks...");
    int networksFound = WiFi.scanNetworks();
    
    if (networksFound == 0) {
        Serial.println("❌ No networks found.");
        return;
    }

    // Construct JSON payload
    String json = "{ \"wifiAccessPoints\": [";
    for (int i = 0; i < networksFound; i++) {
        json += "{ \"macAddress\": \"" + WiFi.BSSIDstr(i) + "\", \"signalStrength\": " + String(WiFi.RSSI(i)) + " }";
        if (i < networksFound - 1) json += ", ";
    }
    json += "] }";
    
    Serial.println("📡 Sending data to Google API...");
    
    // Send HTTP request
    HTTPClient http;
    String url = "https://www.googleapis.com/geolocation/v1/geolocate?key=" + String(apiKey);
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(5000); // Set HTTP timeout to 5 seconds

    int httpResponseCode = http.POST(json);
    
    if (httpResponseCode > 0) {
        Serial.println("✅ Received response:");
        Serial.println(http.getString());  // Contains latitude & longitude
    } else {
        Serial.print("❌ Error in HTTP request: ");
        Serial.println(httpResponseCode);
    }
    
    http.end();
}

// Function to ensure WiFi stays connected
void ensureWiFiConnected() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("⚠️ WiFi Disconnected! Reconnecting...");
        WiFi.disconnect();
        WiFi.begin(ssid, password);

        int attempt = 0;
        while (WiFi.status() != WL_CONNECTED && attempt < 15) {
            Serial.print(".");
            delay(1000);
            attempt++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n✅ Reconnected to WiFi!");
            Serial.print("📍 ESP32 IP Address: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("\n❌ WiFi Connection Failed! Restarting...");
            ESP.restart();
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("🔄 Connecting to WiFi...");
    WiFi.begin(ssid, password);

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 15) {
        Serial.print(".");
        delay(1000);
        attempt++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ Connected to WiFi!");
        Serial.print("📍 ESP32 IP Address: ");
        Serial.println(WiFi.localIP());

        // Get location after connection
        getLocation();
    } else {
        Serial.println("\n❌ WiFi Connection Failed! Restarting...");
        ESP.restart();
    }

    // Check for internet connectivity
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("✅ Internet access confirmed!");
    } else {
        Serial.println("❌ No internet access! Check hotspot settings.");
    }
}

void loop() {
    ensureWiFiConnected();
    getLocation();
    delay(60000);  // Request location every 60 seconds
}
