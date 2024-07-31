//Include the Arduino stepper library
#include <Stepper.h>
#include <WiFi.h>
#define EN_PIN    5 //enable (CFG6)
#define DIR_PIN   2 //direction
#define STEP_PIN  4 //step

//Define an input pin
int buttonPressPin1 = 23;
int buttonPressPin2 = 22;

//Define a Boolean to track a button press
bool buttonPressed1;
bool buttonPressed2;
bool ostatnioZasloniete = true;

const char* ssid     = "";
const char* password = "";

WiFiServer server(80);


//Set how many steps it takes to make a full revolution. Divide the degrees per step by 360 to get the steps
const int stepsPerRevolution = 2048;

//Use pin 8-11 on the arduino to IN1-IN4 on the stepper board
//Setting up the stepper
Stepper silnik = Stepper(stepsPerRevolution, 5,2 , 4, 15);

void setup() {
//set pin modes
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH); //deactivate driver (LOW active)
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW); //LOW to CCW
  //digitalWrite(DIR_PIN, HIGH); //HIGH to CW
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(STEP_PIN, LOW);

  digitalWrite(EN_PIN, LOW); //activate driver


  //Set the RPM of the stepper motor
  //silnik.setSpeed(15);
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
  delay(10);
  // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
  //Set the pinMode of our button pin
  pinMode(buttonPressPin1, INPUT_PULLUP);
  pinMode(buttonPressPin2, INPUT_PULLUP);
  //Set our button press Boolean to a known value
  buttonPressed1 = true;
  buttonPressed2 = true;
}

void loop() {
    WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn the LED on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED off.<br>");
            client.print("Kliknij <a href=\"/O\">tutaj</a> zeby odslonic rolety.<br>");
            client.print("Kliknij <a href=\"/PO\">tutaj</a> zeby odslonic rolety do polowy.<br>");
            client.print("Kliknij <a href=\"/PZ\">tutaj</a> zeby zaslonic rolety do polowy.<br>");
            client.print("Kliknij <a href=\"/Z\">tutaj</a> zeby zaslonic rolety.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /O")) {
          if(ostatnioZasloniete == true)
          {
              buttonPressed1 = false;               // odslania zaslony
              if (buttonPressed1 == false)
            {
              digitalWrite(DIR_PIN, LOW);
                for(int i = 0; i<=1600*14; i++)
              {
                digitalWrite(STEP_PIN, HIGH);
                delay(2);
                digitalWrite(STEP_PIN, LOW);
                delay(2);
              }      
              delay(50);
              buttonPressed1 = true;
              ostatnioZasloniete = false;
            }
          } 
        }
        if (currentLine.endsWith("GET /Z")) {
          if(ostatnioZasloniete == false)
          {
            buttonPressed2 = false;               // zaslania zaslony
            if(buttonPressed2 == false) 
            {
              digitalWrite(DIR_PIN, HIGH);
                for(int i = 0; i<=1600*14; i++)
              {
                digitalWrite(STEP_PIN, HIGH);
                delay(2);
                digitalWrite(STEP_PIN, LOW);
                delay(2);
              }      
              delay(50);
              buttonPressed2 = true;
              ostatnioZasloniete = true;
            }
          }
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
    //Checking the state of the button pin and saving it in our Boolean
    buttonPressed1 = digitalRead(buttonPressPin1);
    buttonPressed2 = digitalRead(buttonPressPin2);
    if(ostatnioZasloniete == true)    // odslania zaslony
    {
      if (buttonPressed1 == false)
      {
        digitalWrite(DIR_PIN, LOW);
          for(int i = 0; i<=1600*14; i++)
        {
          digitalWrite(STEP_PIN, HIGH);
          delay(2);
          digitalWrite(STEP_PIN, LOW);
          delay(2);
        }      
        delay(50);
        buttonPressed1 = true;
        ostatnioZasloniete = false;
      }
    } 
    if(ostatnioZasloniete == false)
    {
      if(buttonPressed2 == false)     // zaslania zaslony
      {
        digitalWrite(DIR_PIN, HIGH);
          for(int i = 0; i<=1600*14; i++)
        {
          digitalWrite(STEP_PIN, HIGH);
          delay(2);
          digitalWrite(STEP_PIN, LOW);
          delay(2);
        }      
        delay(50);
        buttonPressed2 = true;
        ostatnioZasloniete = true;
      }
    }
    //If the button is pressed (grounded), step to 90* and reset the button pressing state
    //odsłonięte rolety
    
}