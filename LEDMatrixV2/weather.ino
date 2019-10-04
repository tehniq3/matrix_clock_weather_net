/*************************************(openweathermap.org)**************************************/

const char *weatherHost = "api.openweathermap.org";

void getWeatherData()
{
  Serial.print("connecting to "); Serial.println(weatherHost);
  if (client.connect(weatherHost, 80)) {
    client.println(String("GET /data/2.5/weather?id=") + cityID + "&units=metric&appid=" + weatherKey + weatherLang + "\r\n" +
                   "Host: " + weatherHost + "\r\nUser-Agent: ArduinoWiFi/1.1\r\n" +
                   "Connection: close\r\n\r\n");
  } else {
    Serial.println("connection failed");
    return;
  }
  String line;
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    Serial.println("w.");
    repeatCounter++;
  }
  while (client.connected() && client.available()) {
    char c = client.read();
//    if (c == '[' || c == ']') c = ' ';  // Leh ergibt Fehler, wenn 2 weather id kommen als array
    line += c;
  }

  client.stop();

  // Test Leh
  // line =  "{\"coord\":{\"lon\":8.55,\"lat\":47.37},\"weather\":[{\"id\":741,\"main\":\"Fog\",\"description\":\"fog\",\"icon\":\"50n\"},{\"id\":701,\"main\":\"Mist\",\"description\":\"mist\",\"icon\":\"50n\"}],\"base\":\"stations\",\"main\":{\"temp\":5.73,\"pressure\":1017,\"humidity\":100,\"temp_min\":5,\"temp_max\":7},\"visibility\":1100,\"wind\":{\"speed\":2.1,\"deg\":140},\"clouds\":{\"all\":75},\"dt\":1541744400,\"sys\":{\"type\":1,\"id\":6016,\"message\":0.0034,\"country\":\"CH\",\"sunrise\":1541744486,\"sunset\":1541779032},\"id\":2657896,\"name\":\"Zurich\",\"cod\":200}";
  Serial.println(line);  // Leh

  // Allocate the memory pool on the stack.
  // Don't forget to change the capacity to match your JSON document.
  // Use arduinojson.org/assistant to compute the capacity.
  //  StaticJsonBuffer<1200> jsonBuf;

  // Allocate JsonBuffer  // Leh
  // Use arduinojson.org/assistant to compute the capacity.
  const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 420;
  DynamicJsonBuffer jsonBuf(capacity);
  
//  DynamicJsonBuffer jsonBuf;
  JsonObject &root = jsonBuf.parseObject(line);
  if (!root.success())
  {
    Serial.println("parseObject() failed");
    return;
  }
  //weatherMain = root["weather"]["main"].as<String>();
  weatherDescription = root["weather"]["description"].as<String>();
  weatherDescription.toLowerCase();
  //  weatherLocation = root["name"].as<String>();
  //  country = root["sys"]["country"].as<String>();
  temp = root["main"]["temp"];
  humidity = root["main"]["humidity"];
  pressure = root["main"]["pressure"];
  tempMin = root["main"]["temp_min"];
  tempMax = root["main"]["temp_max"];
  windSpeed = root["wind"]["speed"];
  windDeg = root["wind"]["deg"];
  clouds = root["clouds"]["all"];
  
 getTimeLocal();

  String windDirection = "N NOO SOS SWW NW";  // Windrichtungen N NO O SO S SW W NW immer 2 char lang
  int wr = (windDeg+22)%360/45;
//  Serial.println(wr);

  String dayName[] = {"Err", "Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
  
  String deg = String(char('~' + 25));
  String arrowUp = String(char('~' + 23));
  
  weatherString = "       " + dayName[w]  + "  " + String(d) + "." + String(mo) + "." + String(ye)+ "      ";
  weatherString +=   String(temp, 1) + deg + "C ";
  weatherString += "  Luftf.: " + String(humidity) + "%  ";  // Luftfeuchtigkeit
  weatherString += "  Luftd: " + String(pressure) + "hPa  "; // Luftdruck
  weatherString += "  Regenrisiko: " + String(clouds) + "%  "; // Regenrisiko
//  weatherString += "  Wind: " + String(windSpeed, 1) + "m/s                "; // Windgeschwindigkeit
  weatherString += "  Wind: " + String(windSpeed, 1) + "m/s  "; // Windgeschwindigkeit
  weatherString += String(windDeg) + deg + "  ";
  weatherString +=  arrowUp + "  " + windDirection.substring(2*wr,2*wr+2) + "                "; // Windgeschwindigkeit und Richtung
  Serial.println(weatherString);  //Leh
}


// =======================================================================
