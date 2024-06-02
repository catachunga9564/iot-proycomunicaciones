/*
Ultima modificación: 20240521
*/
#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <SPI.h>
#include <SD.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "time.h"
#include <HardwareSerial.h>
// Definiciones de constantes
#define DHTTYPE  DHT22
#define DHTPIN    4
#define WIFI_SSID "CAMILO_PC"
//#define WIFI_SSID "Jhoa"
#define WIFI_PASSWORD "12345678"
//#define WIFI_SSID "Tenda_2CD2E0"
//#define WIFI_PASSWORD "Camilo19979798"
#define API_KEY "AIzaSyA3VLtTrmpkoYrcS_G1-1wdqLcIQrfJJ8U"
#define USER_EMAIL "camilo.catachunga@correounivalle.edu.co"
#define USER_PASSWORD "Camilo19979798"
#define DATABASE_URL "iot-proycomunicaciones-default-rtdb.firebaseio.com"
#define SEALEVELPRESSURE_HPA (1018)
/* VARIABLES PARA SIM808*/
HardwareSerial SIM808(1);  //Seleccionamos los pines 7 como Rx y 8 como Tx
const int MySerialRX = 16;
const int MySerialTX = 17;
String incomingMessage;

/*VARIABLES PARA SENSOR MQ135*/
#define         MQ_PIN                       (34)     //define which analog input channel you are going to use
#define         RL_VALUE                     (10)    //define the load resistance on the board, in kilo ohms
#define         RO_CLEAN_AIR_FACTOR          (9.21)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,

#define         CALIBARAION_SAMPLE_TIMES     (50)    //define how many samples you are going to take in the calibration phase
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   //define the time interal(in milisecond) between each samples in the
                                                     //cablibration phase
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interal(in milisecond) between each samples in 
#define         GAS_CO2                      (0)
 
float           CO2Curve[3]  =  {2.3, 0.93,-1.44};
float Ro=10;                  //Ro is initialized to 10 kilo ohms
/*
//////////////CONFIG DNS       
IPAddress local_IP(192, 168, 2, 106);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8,8,4,4);   // add DNS
//////////////////
*/
// Definiciones de PINES ESP32
const int led = 2;
const int bat=35;
File myFile;
const int CS = 5;
// Definición de pines para el LED RGB de cátodo común
const byte pinRed = 12;   // Pin para el color rojo
const byte pinGreen = 13; // Pin para el color verde
const byte pinBlue = 14;  // Pin para el color azul
byte currentColor = 0; // 0: Rojo, 1: Verde, 2: Azul

// Variables globales
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String uid;
String databasePath, tempPath, qualPath, humPath, presPath, ledPath,altPath,ultimoReportePath,voltajeBatPath;
String formattedDate;
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 1500; // Delay para envio de datos
unsigned long tiempo=0;
unsigned long previousMillis = 0;
const long interval = 1000; // Intervalo de tiempo entre cambios de color (en milisegundos)
///////////////VARIABLES PARA NTP - RTC
const char* ntpServer = "co.pool.ntp.org";
const long  gmtOffset_sec = -18000;
const int   daylightOffset_sec = 0;

const unsigned long intervalo = 1 * 300 * 1000000; //*************************** TIEMPO PARA GRABACION DE DATOS**************************//
const unsigned long intervalo2 = 15 * 1000000;       //*************************** TIEMPO PARA ENVÍO DE DATOS**************************//
//////////////////////////////// VARIABLES QUE ALMACENAN DATOS DE LOS SENSORES
int temperature,temperature2;
float humidity,quality,pressure,altitude,promedioTemp,CO2_ppm,sensorValue,voltaje;
String hora="";

////////////////////////////////////////
DHT dht(DHTPIN, DHTTYPE, 22);
Adafruit_BMP280 bmp;
///////////////////////////////
void setRGB(byte redValue, byte greenValue, byte blueValue) {
  // Apagar todos los colores
  digitalWrite(pinRed, redValue);
  digitalWrite(pinGreen, greenValue);
  digitalWrite(pinBlue, blueValue);
}
String getFormattedDate() {
  struct tm timeinfo; // Crear una estructura tm para contener la información de tiempo
  if (!getLocalTime(&timeinfo)) { // Obtener la hora local y verificar si falla
    Serial.println("Failed to obtain time"); // Imprimir mensaje de error
    return ""; // Devolver una cadena vacía en caso de fallo
  }
  
  char formattedDate[9]; // Crear un array de caracteres para contener la fecha formateada (formato AAAAMMDD)
  snprintf(formattedDate, sizeof(formattedDate), "%04d%02d%02d", 
           timeinfo.tm_year + 1900, // Año desde 1900, así que se le suma 1900 para obtener el año actual
           timeinfo.tm_mon + 1, // Meses desde enero, así que se le suma 1 para obtener el mes actual
           timeinfo.tm_mday); // Día del mes
  
  return String(formattedDate); // Convertir la fecha formateada a una String y devolverla
}

String obtenerHora() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "";
  }
  char horaString[20]; // Buffer para almacenar la fecha y hora como cadena (formato AAAA-MM-DD hh:mm:ss)
  sprintf(horaString, "%04d-%02d-%02d %02d:%02d:%02d", 
          timeinfo.tm_year + 1900, // Años desde 1900
          timeinfo.tm_mon + 1,     // Meses desde 0
          timeinfo.tm_mday, 
          timeinfo.tm_hour, 
          timeinfo.tm_min, 
          timeinfo.tm_sec);
  return String(horaString);
}

void WriteInit(String path2){
  myFile = SD.open("/"+path2+".csv", FILE_APPEND);
  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print("hora,temperatura,humedad_relativa");
    myFile.print("\r\n");
    myFile.close(); // close the file:
    Serial.println("*********Se ha escrito el encabezado OK.*************");
  } 
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening file ");
  }
}

void WriteFile(String path, float temp, float hum, String hour) {
  myFile = SD.open("/" + path + ".csv", FILE_APPEND);
  String dataString = "";
  dataString+=hour;
  dataString+=",";
  dataString+=String(temp);
  dataString+=",";
  dataString+=String(hum);

  if (myFile) {
  myFile.println(dataString);  
  myFile.close();
  }
    else {
    Serial.println("error opening file ");
  }
}

void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  digitalWrite(led,HIGH);
}

void sendFloat(String path, float value){
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)){
    //Serial.println("PATH: " + fbdo.dataPath());
    //Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    setRGB(255,0,0);
    Serial.println(".................................RECONECTANDO 1................................................");
    //Serial.println("REASON: " + fbdo.errorReason());
  }
}
void sendString(String path, String value){
  if (Firebase.RTDB.setString(&fbdo, path.c_str(), value)){
    // Serial.println("PATH: " + fbdo.dataPath());
    // Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    setRGB(255,0,0);
    Serial.println(".................................RECONECTANDO 1................................................");
    // Serial.println("REASON: " + fbdo.errorReason());
  }
}

void readData(String path){
  String readIncoming ="";
  if (Firebase.RTDB.getString(&fbdo, path.c_str())){
    //Serial.println("PATH: " + fbdo.dataPath());
    //Serial.println("TYPE: " + fbdo.dataType());
    if(fbdo.dataType()=="string"){
      readIncoming =fbdo.stringData();
      //Serial.println("DATA: " + readIncoming);
      digitalWrite(led, (readIncoming == "ON") ? HIGH : LOW);
    }
  }
  else {
    //digitalWrite(pinRed,HIGH);
    Serial.println(".................................RECONECTANDO 2................................................");
        //Serial.println("REASON: " + fbdo.errorReason());
  }
}

/////////////////////////TIMER 1
hw_timer_t *timer = NULL;
volatile bool has_expired = false; // 
void IRAM_ATTR timerInterrupcion() {
 has_expired = true;
}
/////////////////////////TIMER 2
hw_timer_t *timer2 = NULL;
volatile bool has_expired2 = false; // 
void IRAM_ATTR timerInterrupcion2() {
 has_expired2 = true;
}
///////////////////////////

void setup(){

  SIM808.begin(115200, SERIAL_8N1, MySerialRX, MySerialTX);
  Serial.begin(115200); // Comunicación serial para monitor serial
  dht.begin();
  bmp.begin(0x76);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Ro = MQCalibration(MQ_PIN);  
  // Inicializar los pines como salidas
  pinMode(led, OUTPUT);
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);
  
 // WiFi.config(local_IP, gateway, subnet, dns); // Descomentar si se va a usar DNS
  initWiFi();
  digitalWrite(led,LOW);
  //////////////////////////// TIMER 1
  timer = timerBegin(0, 80, true); // Timer 0, divisor de reloj 80
  timerAttachInterrupt(timer, &timerInterrupcion, true); // Adjuntar la función de manejo de interrupción
  timerAlarmWrite(timer, intervalo, true); // Interrupción cada 1 minuto
  timerAlarmEnable(timer); // Habilitar la alarma
  ////////////////////
  //////////////////////////// TIMER 2
  timer2 = timerBegin(1, 80, true); // Timer 0, divisor de reloj 80
  timerAttachInterrupt(timer2, &timerInterrupcion2, true); // Adjuntar la función de manejo de interrupción
  timerAlarmWrite(timer2, intervalo2, true); // Interrupción cada 2 segundos
  timerAlarmEnable(timer2); // Habilitar la alarma
  ////////////////////
  while (!Serial) { ; }  // wait for serial port to connect. Needed for native USB port only
  Serial.println("Initializing SD card...");
  if (!SD.begin(CS)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  formattedDate = getFormattedDate();
  WriteInit(formattedDate); //////////////Encabezado

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096); //4096 // 2048
  config.token_status_callback = tokenStatusCallback;
  config.max_token_generation_retry = 5;
  Firebase.begin(&config, &auth);
  
  Serial.println("Obteniendo UID de usuario");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  uid = auth.token.uid.c_str();
  Serial.print("UID de usuario: ");
  Serial.println(uid);
  
  databasePath = "/UsersData/" + uid;
  tempPath = databasePath + "/temperature";
  qualPath = databasePath + "/quality";
  humPath = databasePath + "/humidity";
  presPath = databasePath + "/pressure";
  altPath = databasePath + "/altitude";
  ultimoReportePath = databasePath + "/UltimoReporte";
  voltajeBatPath = databasePath + "/voltaje";
  //reportPath = databasePath + "/report";
  ledPath = databasePath + "/led";
/*INICIALIZACION SIM808*/
  // Comprobar comunicación AT
  SIM808.println("AT");
  delay(100);
  if (SIM808.available()) {
    Serial.println(SIM808.readString());
  }

  // Configurar el módulo para enviar SMS
  SIM808.println("AT+CMGF=1"); // Modo texto
  delay(100);
  if (SIM808.available()) {
    Serial.println(SIM808.readString());
  }
    // Elimina todos los mensajes SMS previos
  SIM808.println("AT+CMGDA=\"DEL ALL\"");
  delay(1000);
  SIM808.println("AT+CNMI=1,2,0,0,0");
  delay(1000);

  Serial.println("Listo para recibir mensajes.");
  // Configura el SIM808 para notificar automáticamente cuando llegue un SMS


  // Configurar el número del centro de mensajes (SMSC), si es necesario
  SIM808.println("AT+CSCA=\"+573158603702\""); // Reemplaza con el número de tu operador
  delay(100);
  if (SIM808.available()) {
     Serial.println(SIM808.readString());
   }
  // Ejecutar la función durante 5 segundos
  unsigned long tiempoInicial = millis(); // Obtener el tiempo actual
  while (millis() - tiempoInicial < 1500) { // Ejecutar durante 5 segundos
    parpadeo(500);
  }
  setRGB(0,0,0);
}

void loop(){

  formattedDate = getFormattedDate();
  hora=obtenerHora();
  humidity = dht.readHumidity();
  temperature = (bmp.readTemperature()); //-0.54
  temperature2=(dht.readTemperature()); //+ 0.5
  quality = MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_CO2) +400;
  pressure = (bmp.readPressure()/100)-3.3;
  altitude = bmp.readAltitude(1018); //bmp.readAltitude(1013)
  sensorValue = analogRead(bat);  // Leer el valor analógico del pin 35
  voltaje = sensorValue * (3.3 / 4096.0) * 3.65;  // Ajustar el factor de escala

if(SIM808.available()){
        String sms = "";
    while (SIM808.available()) {
      char c = SIM808.read();
      sms += c;
      delay(10);
    }
    Serial.println("Mensaje recibido:");
    Serial.println(sms);

    // Verifica si el mensaje contiene el texto "datos"
    if (sms.indexOf("datos") != -1) {
      enviarSMS("Respuesta de SIM808: ",temperature2,humidity,pressure,quality,altitude,hora);
    }
  }
  else if (has_expired2 && Firebase.ready()){

      sendFloat(tempPath, temperature2);
      sendFloat(qualPath, quality);
      sendFloat(humPath, humidity);
      sendFloat(presPath, pressure);
      sendFloat(altPath, altitude);
      sendFloat(voltajeBatPath, voltaje);
      sendString(ultimoReportePath, hora); 

      setRGB(0,255,0);
      Serial.println(".................................DATA ENVIADA................................................");
      has_expired2=false; 
  }
  else if(has_expired){

      WriteFile(formattedDate, temperature2, humidity, hora);
      setRGB(0,0,255);
      Serial.println(".................................SD Grabada................................................");
      has_expired=false;
  }

  else{
    setRGB(0,0,0);
  }
  checkWiFiConnection();
}
void enviarSMS(String texto, float temp1, float humi, float presi, float quali, float alti, String horas) {
    // Convertir float a String
  String tempStr = String(temp1, 2); // 2 es el número de decimales
  String humedadStr = String(humi, 2);
  String presiStr = String(presi, 2); // 2 es el número de decimales
  String qualiStr = String(quali, 2); // 2 es el número de decimales
  String altiStr = String(alti, 2); // 2 es el número de decimales
  String horasStr = horas;
  // Concatenar las variables al texto
  texto += " Temperatura: " + tempStr + "*C -";
  texto += " Humedad: " + humedadStr + "% -";
  texto += " Presion: " + presiStr + "hPa -";
  texto += " PPM CO2: " + qualiStr + "PPM -";
  texto += " Altitud: " + altiStr + "msnm -";
  texto += " Hora: " + horasStr;

  SIM808.println("AT+CMGF=1");    // Configura el SIM808 para enviar SMS en formato texto
  delay(500);
  SIM808.println("AT+CMGS=\"+573158603702\""); // 
  delay(500);
  SIM808.print(texto);
  delay(100);
  SIM808.write(26); // Envia el comando Ctrl+Z para enviar el SMS
  delay(2000);
  Serial.println("Mensaje enviado: " + texto);
}

void parpadeo (const long timer){
  unsigned long currentMillis = millis(); // Obtener el tiempo actual
  if (currentMillis - previousMillis >= timer) {
    // Actualizar el tiempo para el siguiente intervalo
    previousMillis = currentMillis;
    // Cambiar al siguiente color
    currentColor = (currentColor + 1) % 3; 
    // Cambiar el color del LED
    switch (currentColor) {
      case 0:
        setRGB(255, 0, 0);   // Rojo
      break;
      case 1:
        setRGB(0, 255, 0);   // Verde
      break;
      case 2:
        setRGB(0, 0, 255);   // Azul
      break;
    }
  }
}
void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conexión WiFi perdida, intentando reconectar...");
    setRGB(255, 0, 0); // Enciende el LED ROJO

    // Intentar reconectar al WiFi
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startAttemptTime = millis();

    // Esperar un máximo de 10 segundos para reconectar
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);
     // Serial.print(".");
    }
  } 
}
 
float MQResistanceCalculation(int raw_adc) {
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}

float MQCalibration(int mq_pin) {
  int i;
  float val=0;
 
  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {            //take multiple samples
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;                   //calculate the average value
  val = val/RO_CLEAN_AIR_FACTOR;                        //divided by RO_CLEAN_AIR_FACTOR yields the Ro 
                                                        //according to the chart in the datasheet 
 
  return val; 
}

float MQRead(int mq_pin) {
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
  rs = rs/READ_SAMPLE_TIMES;
  return rs;  
}
 
int MQGetGasPercentage(float rs_ro_ratio, int gas_id) {
  if ( gas_id == GAS_CO2) {
     return MQGetPercentage(rs_ro_ratio,CO2Curve);
  }  
  return 0;
}

int  MQGetPercentage(float rs_ro_ratio, float *pcurve) {
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}

