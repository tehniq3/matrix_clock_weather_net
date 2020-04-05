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
  Serial.println("-------------");
  String line;
  Serial.println(line);  // Leh
/*
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    Serial.println("w.");
    repeatCounter++;
  }
  */
  while(client.connected() && !client.available()) 
  delay(1);                                          //waits for data
  while (client.connected() && client.available()) {
    char c = client.read();
    if (c == '[' || c == ']') c = ' ';  // Leh ergibt Fehler, wenn 2 weather id kommen als array
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
  cod = root["weather"]["id"];  // cod for general wheater
  //weatherMain = root["weather"]["main"].as<String>();
  weatherDescription = root["weather"]["description"].as<String>();
  weatherDescription.toLowerCase();
  //  weatherLocation = root["name"].as<String>();
  //  country = root["sys"]["country"].as<String>();
  temp = root["main"]["temp"];
  humidity = root["main"]["humidity"];
  pressure = root["main"]["pressure"];
  pressure = pressure *0.75006;
  tempMin = root["main"]["temp_min"];
  tempMax = root["main"]["temp_max"];
  windSpeed = root["wind"]["speed"];
  windDeg = root["wind"]["deg"];
  clouds = root["clouds"]["all"];
  Serial.print("cod = ");
   Serial.println(cod);  //
  Serial.print("Description = ");
   Serial.println(weatherDescription);  //
 getTimeLocal();

 // String windDirection = "N NOO SOS SWW NW";  // Windrichtungen N NO O SO S SW W NW immer 2 char lang
  String windDirection = "N NEE SES SVV NV"; // is "N NE E SE S SV V NV"
  
  int wr = (windDeg+22)%360/45;
//  Serial.println(wr);

//  String dayName[] = {"Err", "Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
  String dayName[] = {"Err", "Duminica", "Luni", "Marti", "Miercuri", "Joi", "Vineri", "Sambata"  };
  
  String deg = String(char('~' + 25));
  String arrowUp = String(char('~' + 23));
  String arrowDown = String(char('~' + 24));
  String arrowRight = String(char('~' + 27));
  String arrowLeft = String(char('~' + 26));
  String arrowUpRight = String(char('~' + 28));
  String arrowUpLeft = String(char('~' + 31));
  String arrowDownRight = String(char('~' + 29));
  String arrowDownLeft = String(char('~' + 30));
   
  weatherString = "       " + dayName[w]  + "  " + String(d) + "." + String(mo) + "." + String(ye)+ "      ";
  weatherString +=   String(temp, 1) + deg + "C ";
  weatherString += "  umiditate: " + String(humidity) + "%  ";  // Luftfeuchtigkeit
  weatherString += "  presiune: " + String(pressure) + "mmHg,  "; // Luftdruck

if ((cod >= 200) and (cod <= 232)) weatherString += "furtuna, ";
if ((cod >= 300) and (cod <= 321)) weatherString += "burnita, ";
if ((cod >= 500) and (cod <= 531)) weatherString += "ploaie, ";
if ((cod >= 600) and (cod <= 622)) weatherString += "ninsoare, ";
if ((cod >= 701) and (cod <= 781)) weatherString += "ceata, ";
if (cod == 800) weatherString += "cer senin, ";
//if ((cod >= 801) and (cod <= 804)) weatherString += "innorat, ";
//if (cod != 800) weatherString += "  nori: " + String(clouds) + "%  "; // Regenrisiko
if ((cod >= 801) and (cod <= 804)) weatherString += "innorat: " + String(clouds) +"%  "; // Regenrisiko;
//if (cod != 800) weatherString += String(clouds) +"%  "; // Regenrisiko

if (windSpeed != 0)
{
//  weatherString += "  Wind: " + String(windSpeed, 1) + "m/s                "; // Windgeschwindigkeit
  weatherString += "  vant: " + String(3.6*windSpeed, 0) + "km/h  "; // Windgeschwindigkeit
//   weatherString +=  arrowUp + "  " + windDirection.substring(2*wr,2*wr+2) + "                "; // Windgeschwindigkeit und Richtung
// weatherString +=  arrowUp + windDirection.substring(2*wr,2*wr+2) + " (" + String(windDeg) + deg + ")                "; // Windgeschwindigkeit und Richtung
if (windDeg>=337.5 || windDeg<=22.5) weatherString +=  arrowUp; // N = 0
if (windDeg>=22.5 && windDeg<=67.6) weatherString +=  arrowUpRight; // NE = 45
if (windDeg>=67.5 && windDeg<=112.5) weatherString +=  arrowRight;  // E = 90
if (windDeg>=112.5 && windDeg<=157.5) weatherString +=  arrowDownRight; // SE = 135
if (windDeg>=157.5 && windDeg<=202.5) weatherString +=  arrowDown; // S = 180
if (windDeg>=202.5 && windDeg<=247.5) weatherString +=  arrowDownLeft; // SV =225
if (windDeg>=247.5 && windDeg<=292.5) weatherString +=  arrowLeft; // V = 270
if (windDeg>=292.5 && windDeg<=337.5) weatherString +=  arrowUpLeft;  // NV = 315
//0 weatherString += arrowUp + arrowUpRight + arrowRight + arrowDownRight + arrowDown + arrowDownLeft + arrowLeft + arrowUpLeft; // just for test
 weatherString +=  windDirection.substring(2*wr,2*wr+2) + " (" + String(windDeg) + deg + ")           "; // Windgeschwindigkeit und Richtung
}
  Serial.println(cod);  //Leh
  Serial.println(weatherString);  //Leh
}


// =======================================================================
