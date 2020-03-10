#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
//#include <RF24_config.h>

//#include <string.h>
//#include <LiquidCrystal_I2C.h>
#include <Entropy.h>
#include <LiquidCrystal.h>
#include "LCDKeypad.hpp"
//#include "Tester.hpp"
#include "Memory.hpp"




char nodeName[nameSize + 1];
///const unsigned char keypadPinValue = 0;
//
enum {DIT, DAH};
const unsigned short timeoutWait = 2000;
const char lettersWithNumbers[62] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
typedef enum {SETUP, MENU, MESSAGE_RECEIVED, CONTACTS, MESSAGES, MESSAGE_NEW, MESSAGE_OPEN, MESSAGE_SENT, MESSAGE_FAILED, NEW_CONTACT, NEW_CONTACT_NAME, NEW_CONTACT_UUID, CONTACT_ADDED, LIST_FULL, ABOUT_ME} State;
char* printText;
State previousState;
unsigned long timer;
typedef enum {CLEAR, NOCLEAR} printStatus;
unsigned long lastMessageReceivedTime;
byte UUID[uuidSize] = "00001";
char* curUUID = "a           ";
char* curNameSoFar = "a          ";
char messageTop;
char messageBottom;
unsigned short messageLength = 0;
char selectedContact[nameSize + 1];
char selectedContactUUID[uuidSize+1];
unsigned short selectedContactIndex =0 ;
unsigned short selectedMessageIndex = 0;
char currentMessengerName[nameSize + 1];
Memory memManager;

LCDKeypad lcd(8, 9, 4, 5, 6, 7);
Button result;
//RF24 radio(A1, A2);
char transmission[uuidSize + uuidSize  + 2 + 1 + 1];

char previousTransmission[uuidSize + uuidSize  + 2 + 1 + 1];
State currentState;
byte address[5]= "00001"; //UUID
RF24 radio(A1, A2);
bool receive = true;
inline void readChar(char h, unsigned short messageLength = 8)
{
  for (unsigned short i = 0; i < messageLength; i++)
  {
    ////Serial.print(((int(h) >> (messageLength - 1 - i)) & 1));
  }
}


inline static void PlaySound(unsigned int ditOrDah)
{
  debounceDelay = 10000;
  if (ditOrDah == DIT)
  {
    tone(buzzerPin, 1000, 100);
  }
  else
  {
    tone(buzzerPin, 1000, 500);
  }
  debounceDelay = 1000;
}


void PlayReceived()
{
tone(buzzerPin, 600, 173);
    delay(250);
    tone(buzzerPin, 600, 173);
    delay(250);
    tone(buzzerPin, 600, 173);
    delay(250);
    tone(buzzerPin, 600, 500);
    delay(250);
    tone(buzzerPin, 455, 550);
    delay(550);
    tone(buzzerPin, 500, 200);
    delay(250);
    tone(buzzerPin, 600, 500);
    delay(250);
    tone(buzzerPin, 500, 173);
    delay(250);
    tone(buzzerPin, 600, 550);
    delay(250);
}

inline void doublePrintOnce(char* text, unsigned short col, unsigned short row, printStatus h)
{
  if (text != printText)
  {
    if (h == CLEAR)
    {
      lcd.clear();

    }
    lcd.setCursor(row, col);
    lcd.print(text);
    if (h == CLEAR)
    {
      ////Serial.println(text);
      ////Serial.println("\n");
    }
    printText = text;
  }
}
inline void readChar2(char a, char b, unsigned short messageLength)
{
  
  char message[17];
  for(unsigned int i = 0; i<17; i++)
  {
    message[i] = ' ';
  }
  ////Serial.print("\nCalling ReadChar2\n");
  if(messageLength<= 8)
  {
    for (unsigned short i = 0; i < messageLength; i++)
    {
      if((((int(a) >> (8 - 1 - i)) & 1)) == 0)
      {
        message[i] = '.';
      }
      else 
      {
        message[i] = '-';
      }
      ////Serial.print(((int(a) >> (8- 1 - i)) & 1));
    }
  }
  else
  {
     for (unsigned short i = 0; i < 8; i++)
    {
      if((((int(a) >> (8- 1 - i)) & 1)) == 0)
      {
        message[i] = '.';
      }
      else
      {
        message[i] = '-';
      }
      ////Serial.print(((int(a) >> (messageLength - 1 - i)) & 1));
    }
      for(unsigned short i =0 ; i< messageLength - 8; i++)
      {
               if((((int(b) >> (8 - 1 - i)) & 1)) == 0)
        {
          message[i+8] = '.';
        }
        else
        {
          message[i+8] = '.';
        }
        ////Serial.print(((int(b) >> (messageLength - 1 - i)) & 1));
        }
    }
    doublePrintOnce(message, 1,0, NOCLEAR);
    
  }





//RADDIIOOOOO COOOOODEEEE ---------------------------------------------------------------------------------------------------------------------------------------
void startReciever(void) {
  radio.startListening();
  radio.openReadingPipe(1, UUID);
  radio.setPALevel(RF24_PA_MIN);

}

bool reciever(void) {
//  Serial.print("\n Calling Receiver\n");

  startReciever();
  if (radio.available()) {
    radio.read(&transmission, sizeof(transmission));
  }
  bool sameTransmission = true;
  for(unsigned int i = 0; i< uuidSize + uuidSize + 3; i++)
  {
    if(transmission[i] != previousTransmission[i])
    {
      sameTransmission = false;
      break;
    }
  }
      
        if((sameTransmission  == false)){
          Serial.print("Transmission is \n");
      
          for(unsigned int i  =0; i<uuidSize + uuidSize + 3; i++)
          {
            Serial.print(byte(transmission[i]), HEX);
                      previousTransmission[i] = transmission[i];
          }
          Serial.print("\n");
          return true;
        }
        else
        {
          return false;
        }
}

void startTransmit(void) {
    radio.stopListening();
  radio.openWritingPipe(address);
  radio.setPALevel(LOW);

}

bool trans(void) {
  startTransmit();
  //     const char transmission[];
  radio.stopListening();
  if (radio.write(&transmission, sizeof(transmission))) { // CHecks whether the message was recieved
    Serial.println("Recieved"); // Prints recieve
    radio.startListening();
  return true;
  }
  else // wasn't recieve
  {
        Serial.println("Failed");
  radio.startListening();
  return false;
    
  }
}

//--------------------------------------------------------------------------------------------------------------






void setup() {
  lastMessageReceivedTime = millis();
  Entropy.initialize();
  // put your setup code here, to run once:
  //  Entropy.initialize();
  Serial.begin(9600);
   pinMode(A1,OUTPUT);
  pinMode(A2, OUTPUT);
  SPI.begin();
  Serial.begin(9600);
  Serial.print("Radio begin!\n");
  radio.begin();
  
  Serial.print("Transmit!\n");
  Serial.print("\nStart Transmit!");  //startTransmit();
  Serial.print("\n Setting details!");
  startReciever();
  radio.setRetries(0,15);
  radio.setAutoAck(true);
char output[uuidSize+1];
Serial.print("\n UUID IS\n");
for(unsigned int i = 0; i<uuidSize;i++)
{
  
   UUID[i] = static_cast<byte>(read(i+3));
  Serial.print(UUID[i], HEX);
}
for(unsigned int i = 0; i<nameSize; i++)
{
  nodeName[i] = read(i + 8);
}
////Serial.print("UUID is: ");
printUUID(UUID, output);
Serial.print("Name is ");
Serial.print(nodeName);
  pinMode(2, OUTPUT);
  lcd.begin(16, 2);
  Serial.print("Number of Messages ");
  Serial.print(read(173));
  Serial.print("\n");
  Serial.print("Number of Contacts ");
  Serial.print(read(20));
  Serial.print("\n");
  timer = millis();

  for (unsigned int i = 0; i < 5; i++)
  {
    lastTimeButtonsChanged[i] = millis();
  }

  //
  //  write(0, 0xC0);
  //  write(1, 0xFF);
  //  write(2, 0xEE);
  ////Serial.print(read(0), HEX);
  ////Serial.print("\n");
  ////Serial.print(read(1), HEX);
  ////Serial.print("\n");
  ////Serial.print( read(2), HEX);
  // write(18, 0xFA);
  // write(19, 0xCE);
  ////Serial.print("\n");
  ////Serial.print("\n");
  ////Serial.print(read(18), HEX);
  ////Serial.print("\n");
  ////Serial.print(read(19), HEX);
  ////Serial.print("\n");
  // write(171, 0xCA);
  // write(172, 0x11);
  ////Serial.print("\n");
  ////Serial.print("\n");
  ////Serial.print(read(171), HEX);
  ////Serial.print("\n");
  ////Serial.print(read(172), HEX);
  ////Serial.print("\n");
  ////Serial.print("\n");


  //write(173, 0);
  //write(20,0);
  //////Serial.print(mem.getNumberContacts());

  //  digitalWrite(173, mem.getNumberMessages());
  if (memManager.hasSchema() == false)
  {
    currentState = SETUP;
  }
  else
  {
    currentState = MENU;
  }
}

State previousUniqueState;// USed for going back to whatever menu you were in after receiving a message and going to the message received screen

void loop() {


   

  switch (result)
  {
    case RIGHT:
//      ////Serial.print("RIGHT");
//      ////Serial.print("\n");
      break;
    case LEFT:
//      ////Serial.print("LEFT");
//      ////Serial.print("\n");
      break;
    case DOWN:
//      ////Serial.print("DOWN");
//      ////Serial.print("\n");
      break;
    case UP:
//      ////Serial.print("UP");
//      ////Serial.print("\n");
      break;
    case SELECT:
//      ////Serial.print("SELECT");
//      ////Serial.print("\n");
      break;
    case NONE:
      break;
  }



  
  State nextState = currentState; 

  
    if(reciever() ==true)
   {
    Serial.print("RECEIVED IS EQUAL TO TRUE!");
    previousUniqueState = currentState;
    nextState = MESSAGE_RECEIVED;
   }
  
  result =  lcd.getButtonPress();
  
  // typedef enum{SETUP, MENU, MESSAGE_RECEIVED, CONTACTS, MESSAGES, MESSAGE_NEW, MESSAGE_OPEN, MESSAGE_SENT, MESSAGE_FAILED, NEW_CONTACT, NEW_CONTACT_NAME, NEW_CONTACT_UUID, CONTACT_ADDED, LIST_FULL, ABOUT_ME} State;
  switch (currentState)
  {
    case SETUP:
      {
        lastMessageReceivedTime = millis();
         doublePrintOnce("Welcome!", 0, 0, NOCLEAR);
         doublePrintOnce("Name: ",1,0,NOCLEAR);
                //GOT TO REDUCE DEBOUNCEDELAY SO THAT THE USER CAN SCROLL MORE QUICKLY
                static unsigned short letterScrollIndex = 0;
                static unsigned short letterAddedIndex = 0;
                // CHECK TO SEE WHAT THE MAX SIZE OF THE NAME IS!!!!!!!!
                static char nameSoFar[nameSize + 1] = "a         ";
        switch (result)
        {
          case UP:
            {
              if (lettersWithNumbers[letterScrollIndex] == 'Z')
              {
                letterScrollIndex = 0;
              }
              else
              {
                letterScrollIndex++;
              }
              nameSoFar[letterAddedIndex] = lettersWithNumbers[letterScrollIndex];
            }
            break;
          case DOWN:
            {
              if (lettersWithNumbers[letterScrollIndex] == 'a')
              {
                letterScrollIndex = 51;
              }
              else
              {
                letterScrollIndex--;
              }
              nameSoFar[letterAddedIndex] = lettersWithNumbers[letterScrollIndex];
            }
            break;
          case RIGHT:
            {

              if (letterAddedIndex == nameSize)
              {
                letterAddedIndex = 0;
              }
              letterAddedIndex++;
              if (letterAddedIndex > 9)
              {
                letterAddedIndex--;
              }
              nameSoFar[letterAddedIndex] = lettersWithNumbers[letterScrollIndex];
              // ////Serial.print(letterAddedIndex);
              //////Serial.print("\n");
            }
            break;
          case LEFT:
            {

              if (letterAddedIndex != 0)
              {
                letterAddedIndex--;
              }
              if (letterAddedIndex == 0)
              {
                // letterAddedIndex = nameSize-1;
              }
              for (short i = letterAddedIndex + 1; i < 11; i++)
                nameSoFar[i] = ' ';
            }
            break;
          case SELECT:
            {
                ////Serial.print("PRESSED SELECT");
                ////Serial.print("Starting SCHEMA");
                memManager.setSchema();
                write(18, 0xFA);
                write(19, 0xCE);
                ////Serial.print("Saving at memory address of");
                  for(unsigned int i = 0; i<uuidSize; i++)
                  {
                    ////Serial.print(3+i);
                    write(3 + i, Entropy.randomByte());
                    UUID[i] = read(3+i);
                  }        
              lcd.clear();
              for(unsigned int i = 0; i<nameSize; i++)
              {
                nodeName[i] = nameSoFar[i];
                write(i+8, nodeName[i]);
              }
//              ////Serial.print("curNameSoFar IS NOW ");
//              ////Serial.print("\n");
//              ////Serial.print(nodeName);
                        ////Serial.print("Name is: ");
              ////Serial.print(nodeName);
                char output[uuidSize + 1];
                ////Serial.print("Personal UUID IS ");
                printUUID(UUID, output);
              ////Serial.print("\n");
              lcd.clear();
              nextState = MENU;
            }
            break;
            default:
            {
                 doublePrintOnce(nameSoFar,1,short(sizeof("Name: "))-1,NOCLEAR);
            }
            break;
        }
      }
      break;
    case MENU:
      {
        static int n = 0;
        if (previousState != MENU)
        {
          ////Serial.print("NUMBER OF MESSAGES ");
          ////Serial.print(read(173));
          ////Serial.print("\n");
          ////Serial.print("NUMBER OF CONTACTS ");
          ////Serial.print(read(20));
          ////Serial.print("\n");
        }
        doublePrintOnce("Menu:", 0, 0, NOCLEAR);  
        switch (result)
        {
          case RIGHT:
            {
              if (n == 3)
              {
                n = 0;
              }
              else
              {
                n = n + 1;
              }
//              ////Serial.print("N is now");
//              ////Serial.print("\n");
//              ////Serial.print(n);
            }
            break;
          case LEFT:
            {
              if (n == 0)
              {
                n = 3;
              }
              else
              {
                n = n - 1;
              }
            }
            break;
          case SELECT:
            {
              if(n==0)
              {
               nextState = CONTACTS; 
              }
              else if(n == 1)
              {
                nextState = MESSAGES;
              }
              else if(n==2)
              {
                nextState = NEW_CONTACT;
              }
              else if(n==3)
              {
                nextState = ABOUT_ME;
              }
              lcd.clear();
            }
            break;
          default:
            {
              if(n == 0)
              {
                doublePrintOnce("<-  Contacts  ->",1,0,NOCLEAR);
              }
              else if(n == 1)
              {
                doublePrintOnce("<-  Messages  ->", 1, 0, NOCLEAR);
              }
              else if(n == 2)
               {
                doublePrintOnce("<- N. Contact ->", 1, 0, NOCLEAR);
               }
               else if(n == 3)
               {
                doublePrintOnce("<-  About Me  ->", 1, 0, NOCLEAR);
               }
            }
            break;
        }
      }
      break;
    case MESSAGE_RECEIVED:
      {
        PlayReceived();
        doublePrintOnce("New Message From: ", 0, 0, NOCLEAR);
         static char senderUUID[uuidSize+1];
        static char receiverUUID[uuidSize+1];
        static char message1;
        static char message2;
        static char messageLength;
        char testUUID[uuidSize+1];
        if(currentState != previousState)
        {
             for(unsigned int i =0; i< uuidSize ; i++)
             {
               senderUUID[i] = transmission[i];
             }
             for(unsigned int i =0; i<uuidSize; i++)
             {
              receiverUUID[i] = transmission[i + uuidSize];
             }
             message1 = transmission[uuidSize + uuidSize];
             message2 = transmission[uuidSize + uuidSize + 1];
             messageLength = uuidSize + uuidSize + 1;
             short memoryOffset = memManager.foundUUID(senderUUID);
             if(memoryOffset == -1)
             {
              doublePrintOnce("Anonymous",1,0,NOCLEAR);
              
             }
             else
             {
              char nameR[nameSize+1];
              for(unsigned int i =0; i<nameSize; i++)
              {
                nameR[i] = byte(read(memoryOffset+i));
              }
              Serial.print("NAME IS nameR");
              Serial.println(nameR);
              doublePrintOnce(nameR, 1,short(sizeof("<- ")) -1, NOCLEAR);
             }
             
                 for(unsigned int i = 0; i<uuidSize; i++)
              {
                ////Serial.print(read(434) + i);
                write(read(434) + i, senderUUID[i]);
                testUUID[i]  = read(read(434) + i);
                ////Serial.print("\n");
              }
//              ////Serial.print("\n PRINITNG UUID THAT HAS JUST BEEN SAVED\n");
//              printUUID(testUUID, output);
              ////Serial.print("\nSaving TO UUID at addresses \n");
              for(unsigned int i = 0; i<uuidSize; i++)
              {
                ////Serial.print(read(434) + i+uuidSize);
                write(read(434) + i+uuidSize, char(receiverUUID[i]));
              }
              
              ////Serial.print("\n Reading TO UUID addresses \n");
              for(unsigned int i = 0; i<uuidSize; i++)
              {
                ////Serial.print(read(434) + i+uuidSize);
                testUUID[i] = read(read(434) + i+uuidSize);
              }
              ////Serial.print("\n");
              //printUUID(testUUID, output);

              
//              ////Serial.print("\nSaving Message at Address of ");
//              ////Serial.print(read(434) + uuidSize + uuidSize);
//              ////Serial.print(read(434) + uuidSize + uuidSize + 1);
//              ////Serial.print("\n MESSAGES ARE \n");
//              ////Serial.print(messageTop, BIN);
//              ////Serial.print(messageBottom, BIN);
              write(read(434) + uuidSize + uuidSize, message1);
              ////Serial.print(read(read(434) + uuidSize + uuidSize), BIN);
              write(read(434)+ uuidSize + uuidSize + 1, message2);
              ////Serial.print(read(read(434) + uuidSize + uuidSize+1), BIN);
              write(read(434)+ uuidSize + uuidSize + 2, messageLength);
              if(read(434) == 421)
              {
                write(434, 174);
              }
              else
              {
                write(434, read(434)+13);
              }
              write(173, read(173) + 1);
        }
//
//             
//
                 static bool savedMessage = false;
        if(previousState != MESSAGE_RECEIVED)
        {
          savedMessage = false;
          timer = millis();
        }
//        lcd.clear();
//          doublePrintOnce("MESSAGE RECEIVED", 0, 0, NOCLEAR);
        while(millis() - timer < timeoutWait)
        {
          result  = lcd.getButtonPress();
          if(result == UP)
          {
            savedMessage = true;
                ////Serial.print("\n STILL NOT 2 SECOINDS");
                ////Serial.print("UUUUUUP\n");
                              char output[uuidSize+1];
                              char toUUID[uuidSize + 1];
    //              ////Serial.print("SelectedContactIndex is ");
    //              ////Serial.print(selectedContactIndex);
    //              ////Serial.print("\n");
    //              ////Serial.print(selectedContactIndex);
    //              ////Serial.print("Name is ");
    //              
                  for(unsigned int i =0; i<uuidSize; i++)
                  {
    //                 ////Serial.print(" Getting UUID at address of ");
    //                ////Serial.print(21 + selectedContactIndex* 15+i);
    //                ////Serial.println(char(read(21 + selectedContactIndex* 15+i)), HEX);
                    toUUID[i] = read(21 + selectedContactIndex*15 + i);
    //                
                  }
    //              ////Serial.print("NODE UUID IS ACTUALLY BEING SAVED BY ");
    //              printUUID(UUID, output);
    //              ////Serial.print("\n");
    //              ////Serial.print("\n TO UUID IS ");
    //              printUUID(toUUID, output);
                  ////Serial.print("\n");
    //              for(unsigned int i =0; i<nameSize; i++)
    //              {
    //                ////Serial.print(" Printing address of ");
    //                ////Serial.print(21 + selectedContactIndex* 15+i + 5);
    //                ////Serial.println(char(read(21 + selectedContactIndex* 15+i + 5)));
    //              }
                  ////Serial.print("Writing NODE UUID AT ADDRESS ");
                  char testUUID[uuidSize+1];
                  for(unsigned int i = 0; i<uuidSize; i++)
                  {
                    ////Serial.print(read(434) + i);
                    write(read(434) + i, UUID[i]);
                    testUUID[i]  = read(read(434) + i);
                    ////Serial.print("\n");
                  }
    //              ////Serial.print("\n PRINITNG UUID THAT HAS JUST BEEN SAVED\n");
    //              printUUID(testUUID, output);
                  ////Serial.print("\nSaving TO UUID at addresses \n");
                  for(unsigned int i = 0; i<uuidSize; i++)
                  {
                    ////Serial.print(read(434) + i+uuidSize);
                    write(read(434) + i+uuidSize, char(toUUID[i]));
                  }
                  
                  ////Serial.print("\n Reading TO UUID addresses \n");
                  for(unsigned int i = 0; i<uuidSize; i++)
                  {
                    ////Serial.print(read(434) + i+uuidSize);
                    testUUID[i] = read(read(434) + i+uuidSize);
                  }
                  ////Serial.print("\n");
                  printUUID(testUUID, output);
    
                  
    //              ////Serial.print("\nSaving Message at Address of ");
    //              ////Serial.print(read(434) + uuidSize + uuidSize);
    //              ////Serial.print(read(434) + uuidSize + uuidSize + 1);
    //              ////Serial.print("\n MESSAGES ARE \n");
    //              ////Serial.print(messageTop, BIN);
    //              ////Serial.print(messageBottom, BIN);
                  write(read(434) + uuidSize + uuidSize, messageTop);
                  ////Serial.print(read(read(434) + uuidSize + uuidSize), BIN);
                  write(read(434)+ uuidSize + uuidSize + 1, messageBottom);
                  ////Serial.print(read(read(434) + uuidSize + uuidSize+1), BIN);
                  write(read(434)+ uuidSize + uuidSize + 2, messageLength);
                  if(read(434) == 421)
                  {
                    write(434, 174);
                  }
                  else
                  {
                    write(434, read(434)+13);
                  }
                  write(173, read(173) + 1);
                lcd.clear();
                nextState = previousUniqueState;
                break;
              }
          
         
        }
        lcd.clear();
        
        nextState  = MENU;
      }
      break;
    case CONTACTS:
      {
        static short counter = 0;
        //make it so that you can scroll through contacts!!!!
        static unsigned short memoryOffset = 21 + uuidSize;
        static short previousCounterNum = 9;
        switch (result)
        {
          case UP:
            {
              nextState = MENU;
              lcd.clear();
            }
            break;
          case SELECT:
            {

              lcd.clear();
              nextState = MESSAGE_NEW;
            }
            break;
          case RIGHT:
            {
              previousCounterNum = counter;
              if (counter >= memManager.getNumberContacts() - 1)
              {
                selectedContactIndex = 0;
                counter = 0;
              }
              else
              {
               selectedContactIndex++;
                counter++;
              }
              //lcd.clear();


            }
            break;
          case LEFT:
            {
              previousCounterNum = counter;
              if (counter == 0)
              {
                selectedContactIndex = memManager.getNumberContacts()-1;
                counter = memManager.getNumberContacts() - 1;

              }
              else
              {
                selectedContactIndex--;
                counter--;
              }
            }
            break;
          default:
            {
              static bool getOut = false;
                if(memManager.getNumberContacts() == 0)
                {
                  ////Serial.print("NO CONTACTS\n");
                  if(previousState == MENU)
                  {
                    ////Serial.print("GET OUT AT FALSE\n");
                    getOut = false;
                    timer = millis();
                  }
                  lcd.clear();
                  doublePrintOnce("No contacts!",0,0,NOCLEAR);
                  while(millis() - timer < timeoutWait)
                  {
                    result = lcd.getButtonPress();
                    ////Serial.print("Still not 2 seconds");
                                 if(result == UP)
                      {
                        getOut = true;
                      
                            lcd.clear();
                            nextState = MENU;
                            break;
                          }
                  }
                  
                   if(getOut== false) {
                    ////Serial.print("GET OUT  IS EQUAL TO FALSE");
                           lcd.clear();
                            nextState = MENU;
                           break;
                    }
               
                }
                else
                {
                 doublePrintOnce("Contact:", 0, 0, NOCLEAR); 
                   if ((previousCounterNum != counter)||(previousState!=CONTACTS))
              {
                char nameI[nameSize + 1];
//                ////Serial.print("Number of Contacts is ");
//                ////Serial.print(read(20));
//                ////Serial.print("\n");
                Serial.print("Selected Contact UUID is: \n");
                for(unsigned int i =0; i<uuidSize; i++)
                {
                   selectedContactUUID[i] = read(memoryOffset - uuidSize + counter * 15 + i);
                  if(selectedContactUUID[i]< 0 )
                  {
                    Serial.print(selectedContactUUID[i] + 256, HEX);
                  }
                  else
                  {
                    Serial.print(selectedContactUUID[i], HEX);
                  }
                    
                 
                  
                }
                for (unsigned int i = 0; i < nameSize; i++)
                {
//                  Serial.print("OFFSET IS ");
//                                  Serial.print("\n");
//                  Serial.print(memoryOffset + counter * 15 + i);
//                  Serial.print(" | ");
                  ////Serial.println(char(read(memoryOffset + counter * 15 + i)));
                  if (short(read(memoryOffset + counter * 15 + i)) < 32)
                  {
                    nameI[i] = ' ';
                  }
                  else
                  {
                    nameI[i] = char(read(memoryOffset + counter * 15 + i));
                  }
                }
                doublePrintOnce("<- ", 1, 0, NOCLEAR);
                doublePrintOnce(nameI, 1, short(sizeof("<- ")), NOCLEAR);
                doublePrintOnce(" ->", 1, 13, NOCLEAR);
                for (unsigned int i = 0; i < nameSize; i++)
                {
                  selectedContact[i] = nameI[i];
                }


              }
              previousCounterNum = counter;
                }
            

            }
            break;
        }
      }
      break;
    case MESSAGES:
      {
        static unsigned short counter = 0;
        static unsigned short previousCounter = 19;
        doublePrintOnce("Messages:", 0, 0, NOCLEAR);
        switch (result)
        {
          case UP:
            {
              lcd.clear();
              nextState = MENU;
            }
            break;
          case LEFT:
            {
              ////Serial.print("\n");
              ////Serial.print(char(counter+48));
              ////Serial.print("\n");
              if (counter == 0)
              {
                selectedMessageIndex = memManager.getNumberMessages()-1;
                counter = memManager.getNumberMessages()-1;
              }
              else
              {
                selectedMessageIndex--;
                counter--;
              }
            }
            break;
          case RIGHT:
            {
              if (counter >= memManager.getNumberMessages()-1)
              {
                selectedMessageIndex =0;
                counter = 0;
              }
              else
              {
                selectedMessageIndex++;
                counter++;
              }

            }
            break;
          case DOWN:
            {

            }
            break;
            case SELECT:
            {
              lcd.clear();
              nextState = MESSAGE_OPEN;
            }
          default:
            {
              if(memManager.getNumberMessages() == 0)
              {
                timer = millis();
                lcd.clear();
                doublePrintOnce("No messages!", 0, 0, NOCLEAR);
                while(millis() - timer < timeoutWait)
                {
                  result = lcd.getButtonPress();
                  if(result == UP)
                  {
                    lcd.clear();
                    nextState = MENU;
                    break;
                  }
                  
                }

                lcd.clear();
                nextState = MENU;
                break;                
              }
              else if((counter != previousCounter)||(previousState == MESSAGE_OPEN)||(previousState == MENU))
              {
                //Serial.print("Selected Message Index is \n");
                //Serial.print(counter);
                Serial.print("\n");
                Serial.print("Memory offset is ");
               
                unsigned short memoryOffset = 174 + (selectedMessageIndex* 13);
                 //Serial.print(memoryOffset);
                byte senderUUID[uuidSize+1];
                byte receiverUUID[uuidSize+1];
                                Serial.print("---------------------------------------\n");
                             Serial.print("\nSender UUID is ");
                for (unsigned int i = 0; i < uuidSize; i++)
                {
//                  Serial.print("\n");
//                  Serial.print(" Reading Sender UUID at memory address of ");
//                  Serial.print(memoryOffset+i);
//                  Serial.print("  |  ");
//                  Serial.print(read(memoryOffset + i), HEX);
                  senderUUID[i] =  byte(read(memoryOffset + i));
                  
                }
                                Serial.print("---------------------------------------");
                                Serial.print("\nReceiver UUID is ");
                for (unsigned int i = 0; i < uuidSize; i++)
                {
//                  Serial.print("\n");
//                  Serial.print(" Reading receiver UUID at memory address of ");
//                  Serial.print(memoryOffset+i+uuidSize);
//                  Serial.print("          |  ");
//                  Serial.print(read(memoryOffset+i+ uuidSize), HEX);
                  receiverUUID[i] =  (read(memoryOffset + i+uuidSize));
                }   
                Serial.print("\n PRINTING RECEIVER UUID");      
                for(unsigned int i =0; i< uuidSize; i++)
                {
                  Serial.print(receiverUUID[i], HEX);
                  Serial.print("\n");
                }
                bool sent  = true;
                for (unsigned int i = 0; i < uuidSize; i++)
                {
//                  checkUUID[i] = memoryOffset;
                  if ((senderUUID[i]) != UUID[i])
                  {
//                    ////Serial.print("\n");
//                    ////Serial.print("SENT IS FALSE");
//                    ////Serial.print("\n");
                    sent = false;
                  }
                }
                Serial.print(sent == true ? " SENT MESSAGE" : "RECEIVED MESSAGE");
//                ////Serial.print("\n");
//                ////Serial.print("NODE UUID IS ");
//                ////Serial.print("\n");
//                printUUID(UUID, output);
//                ////Serial.print("\n");
//                ////Serial.print("SENDER UUID IS ");
//                ////Serial.print("\n");
//                printUUID(senderUUID, output);
//                   short nameOffset = memManager.foundUUID(receiverUUID);
                //////Serial.print(memManager.foundUUID(receiverUUID), HEX);
                
                  char nameR[nameSize+1] = "          ";
                Serial.print(memManager.foundUUID(receiverUUID) == true?" FOUND UUID":"UUID NOT FOUND");
                short nameOffset = memManager.foundUUID((sent==true)?receiverUUID:senderUUID);
//                ////Serial.print("NAME OFFSET IS \n");
//                ////Serial.print(nameOffset);


                if(nameOffset != -1 )
                {
                  Serial.print("NAME OFFSET IS \n");
                  Serial.print(nameOffset);
                  for(unsigned int i =0 ; i< nameSize; i++)
                {
                  nameR[i] = char(read(nameOffset + i));
                  currentMessengerName[i] = nameR[i];
                }
                Serial.print("\n NAME IS ");
                Serial.print(nameR);
                  doublePrintOnce((sent==true)?"[S":"[R",0,16-2,NOCLEAR);
                  lcd.setCursor(0,1);
                  lcd.print(selectedMessageIndex+1);
                  lcd.setCursor(((selectedMessageIndex+1)>=10)?2:1,1);
                  lcd.print(".");
                   doublePrintOnce(" ",1,((selectedMessageIndex+1)>=10)?3:2,NOCLEAR);
                  doublePrintOnce(nameR, 1,((selectedMessageIndex+1)>=10)?4:3,NOCLEAR);

                }
                else
                {
                 lcd.clear();
                  lcd.setCursor(0,1);
                  lcd.print(selectedMessageIndex+1);
                  lcd.setCursor(((selectedMessageIndex+1)>=10)?2:1,1);
                  lcd.print(".");
                   doublePrintOnce(" ",1,((selectedMessageIndex+1)>=10)?3:2,NOCLEAR);
                  doublePrintOnce("Anonymous", 1,((selectedMessageIndex+1)>=10)?4:3,NOCLEAR);
                }

                   previousCounter = counter;
                }                
              }
            break;
        }
      }
      break;
    case MESSAGE_NEW:
      {
        //GOT TO REDUCE DEBOUNCEDELAY SO THAT THE USER CAN SCROLL MORE QUICKLY
        static unsigned short letterScrollIndex = 0;
        static unsigned short letterAddedIndex = 0;
        static char message1 = 0;
        static char message2 = 0;
        // CHECK TO SEE WHAT THE MAX SIZE OF THE NAME IS!!!!!!!!
        static char message[] = "                ";
        if(previousState != MESSAGE_NEW)
        {
          letterAddedIndex = 0;
          letterScrollIndex = 0;
          for(unsigned int i =0; i<nameSize; i++)
          {
            message[i] = ' ';
          }
        }
        doublePrintOnce("To: ", 0, 0, NOCLEAR);
        doublePrintOnce(selectedContact, 0, short(sizeof("To: ")) - 1, NOCLEAR);
        switch (result)
        {
          case UP:
            {
              lcd.clear();
              nextState = CONTACTS;
            }
            break;
          case LEFT:
            {

              PlaySound(DIT);

              message[letterAddedIndex] = '.';
              if (letterAddedIndex >= messageSize - 1)
              {
                letterAddedIndex = messageSize - 1;
                if (letterAddedIndex < 8)
                {
                  message1 = message1 & (~(1 << (7 - letterAddedIndex)));
                } else
                {
                  message2 = message2 & (~(1 << (7 - letterAddedIndex - 8)));
                }
              }
              else
              {
                if (letterAddedIndex < 8)
                {
                  message1 = message1 & (~(1 << (7 - letterAddedIndex)));
                } else
                {
                  message2 = message2 & (~(1 << (7 - letterAddedIndex - 8)));
                }
                letterAddedIndex++;
              }
              ////Serial.print("CHARS ARE");
              ////Serial.print("\n");
              readChar(message1);
              ////Serial.print("\n");
              readChar(message2);
              ////Serial.print("\n");
              delay(400);
              // ////Serial.print(letterAddedIndex);
              //////Serial.print("\n");
            }
            break;
          case RIGHT:
            {

              PlaySound(DAH);

              message[letterAddedIndex] = '-';
              if (letterAddedIndex >= messageSize - 1)
              {
                letterAddedIndex = messageSize - 1;
                if (letterAddedIndex < 8)
                {
                  message1 = message1 | ((1 << (7 - letterAddedIndex)));
                } else
                {
                  message2 = message2 | ((1 << (7 - (letterAddedIndex - 8))));
                }
              }
              else
              {
                if (letterAddedIndex < 8)
                {
                  message1 = message1 | ((1 << (7 - letterAddedIndex)));
                } else
                {
                  message2 = message2 | ((1 << (7 - (letterAddedIndex - 8))));
                }
                letterAddedIndex++;
              }

              ////Serial.print("CHARS ARE");
              ////Serial.print("\n");
              readChar(message1);
              ////Serial.print("\n");
              readChar(message2);
              ////Serial.print("\n");
              delay(500);
              // ////Serial.print(letterAddedIndex);
              //////Serial.print("\n");
            }
            break;
          case DOWN:
            {

              if (letterAddedIndex != 0)
              {
                message[letterAddedIndex] = ' ';
                letterAddedIndex--;
              }


            }
            break;
          case SELECT:
            {
              char testUUID[uuidSize+1];
              char output[uuidSize+1];

              char toUUID[uuidSize+1];
              Serial.print(" TO UUID IS ");
              for(unsigned int i = 0; i< uuidSize; i++)
              {
                if(selectedContactUUID[i] < 0)
                {
                  toUUID[i] = selectedContactUUID[i] + 256;
                Serial.print(selectedContactUUID[i] + 256, HEX);
                }
                else
                {
                   toUUID[i] = selectedContactUUID[i];
                    Serial.print(selectedContactUUID[i], HEX);
                }
                
              }
              
              messageTop = message1;
              messageBottom = message2;
              messageLength = letterAddedIndex + 1;
              receive = false;
              Serial.print("Address is now:!\n");
              for(unsigned int i = 0; i<uuidSize; i++)
              {
                Serial.print(toUUID[i], HEX);
              }
              Serial.print("\n");              
              for(unsigned int i =0; i<uuidSize; i++)
              {
                address[i] =  static_cast<byte>(toUUID[i]);
                Serial.print(address[i], HEX);
              }
              
              Serial.print("\n");
              for(unsigned int i =0; i<uuidSize; i++)
              {
                transmission[i] = UUID[i];
              }
              for(unsigned int i =0; i<uuidSize; i++)
              {
                transmission[i+uuidSize] = address[i];
              }
              transmission[uuidSize + uuidSize] = messageTop;
              transmission[uuidSize + uuidSize + 1] = messageBottom;
              transmission[uuidSize + uuidSize + 2] = messageLength;
              
              timer = millis();
              bool received = false;
              while(millis() - timer < 10000)
              {
                Serial.print("\n Sending!\n");
                  if(trans() == true)
                {
                  received = true;
                  break;            
                }
              }
              
              if(received)
              {
                       startReciever();
                nextState = MESSAGE_SENT;
                     lcd.clear();
              }
              else
              {
                  lcd.clear();
                    startReciever();
                nextState = MESSAGE_FAILED;
              }
                  
            }
            break;
          default:
            {
              if ( letterAddedIndex == 0) {
                doublePrintOnce(message, 1, 0, NOCLEAR);
                break;
              }
              for (unsigned int i = 0; i < letterAddedIndex; i++)
              {
                char* helpMe = "heeelp";
                doublePrintOnce(message, 1, 0, NOCLEAR);
                //lcd.print(nameSoFar[i]);

              }
            }
            break;
        }
      }
      break;
    case MESSAGE_OPEN:
    {
      static unsigned char counter = 0;
      static unsigned char previousCounter = 19;
      if(counter != previousCounter)
      {
        unsigned short memoryOffset = 174 + (selectedMessageIndex* 13);
                char senderUUID[uuidSize + 1];
                for (unsigned int i = 0; i < uuidSize; i++)
                {
//                  ////Serial.print("\n");
//                  ////Serial.print(" Reading Sender UUID at memory address of ");
//                  ////Serial.print(memoryOffset+i);
                  senderUUID[i] =  read(memoryOffset + i);
                }
                char receiverUUID[uuidSize+1];
                                ////Serial.print("---------------------------------------");
                                ////Serial.print("\nReceiver UUID is ");
                for (unsigned int i = 0; i < uuidSize; i++)
                {
//                  ////Serial.print("\n");
//                  ////Serial.print(" Reading receiver UUID at memory address of ");
//                  ////Serial.print(memoryOffset+i+uuidSize);
//                  ////Serial.print("          |  ");
//                  ////Serial.print(read(memoryOffset+i+ uuidSize), HEX);
                  receiverUUID[i] =  char(read(memoryOffset + i+uuidSize));
                  ////Serial.print(receiverUUID[i], HEX);
                }   
                bool sent = true;
                char checkUUID[6];
                for (unsigned int i = 0; i < uuidSize; i++)
                {
                  checkUUID[i] = memoryOffset;
                  if ((senderUUID[i]) != UUID[i])
                  {
//                    ////Serial.print("\n");
//                    ////Serial.print("SENT IS FALSE");
//                    ////Serial.print("\n");
                    sent = false;
                  }
                }
                ////Serial.print(sent == true ? " SENT MESSAGE" : "RECEIVED MESSAGE");
                char output[6];
//                ////Serial.print("\n");
//                ////Serial.print("NODE UUID IS ");
//                ////Serial.print("\n");
//                printUUID(UUID, output);
//                ////Serial.print("\n");
//                ////Serial.print("SENDER UUID IS ");
//                ////Serial.print("\n");
                printUUID(senderUUID, output);
                if (sent == true)
                {

                  doublePrintOnce("[S", 0, 14, CLEAR);
                  doublePrintOnce("Sent: ",0,0,NOCLEAR);
//                  ////Serial.print("RECEIVER UUID IS \n");
//                  for(unsigned int i =0 ; i< uuidSize; i++)
//                  {
//                    ////Serial.print(receiverUUID[i], HEX);
//                  }
//                  short nameOffset = memManager.foundUUID(receiverUUID);
//                  ////Serial.print("\nNAME OFFSET IS \n");
                  //////Serial.print(nameOffset);
//                    for(unsigned int i = 0; i< nameSize; i++)
//                    {
//                      nameR[i] = read(i + nameOffset);
//                    }
                    doublePrintOnce(currentMessengerName,0,short(sizeof("Sent: ")-1), NOCLEAR);
                }
                else
                {
                  Serial.print("FROM: ");
                  lcd.clear();
                  doublePrintOnce("From: ", 0, 0, NOCLEAR);
                  doublePrintOnce(currentMessengerName,0,short(sizeof("From: ")-1), NOCLEAR);
                }

                
                //doublePrintOnce(
                
//                ////Serial.print("\n");
//                ////Serial.print("Accessing Message at Offset ");
//                ////Serial.print(memoryOffset + 2 * uuidSize);
//                ////Serial.print(" and ");
//                ////Serial.print(memoryOffset + 2 * uuidSize + 1);
                unsigned short messageLength = read(memoryOffset + 2 * uuidSize + 2);
//                ////Serial.print("\n");
//                ////Serial.print("Message Length is ");
//                ////Serial.print(messageLength);
//                ////Serial.print("Message is -----------------------------\n");
//                ////Serial.print("\n Reading the char at memory offset of ");
//                ////Serial.print(memoryOffset + 2 * uuidSize);
//                ////Serial.print("\n");
//                ////Serial.print(memoryOffset + 2 * uuidSize+1);
//                ////Serial.print("\n");
//                ////Serial.print(read(memoryOffset + 2 * uuidSize), BIN);
//                ////Serial.print("\n");
//                ////Serial.print(read(memoryOffset + 2 * uuidSize+1), BIN);
                readChar2(char(read(memoryOffset + 2 * uuidSize)),char(read(memoryOffset + 2 * uuidSize+1)) , messageLength-1);
        previousCounter = counter;
      }
      switch(result)
      {
        
        case UP:
        {
          counter = 0;
          previousCounter = 19;
          lcd.clear();
          nextState = MESSAGES;
        }
        break;
      }
    }
      break;
    case MESSAGE_SENT:
      {
        static bool savedMessage = false;
        if(previousState != MESSAGE_SENT)
        {
          savedMessage = false;
          timer = millis();
        }
          doublePrintOnce("MESSAGE SENT", 0, 0, NOCLEAR);
        while(millis() - timer < timeoutWait)
        {
          result  = lcd.getButtonPress();
          if(result == UP)
          {
            savedMessage = true;
                ////Serial.print("\n STILL NOT 2 SECOINDS");
                ////Serial.print("UUUUUUP\n");
                              char output[uuidSize+1];
                              char toUUID[uuidSize + 1];
    //              ////Serial.print("SelectedContactIndex is ");
    //              ////Serial.print(selectedContactIndex);
    //              ////Serial.print("\n");
    //              ////Serial.print(selectedContactIndex);
    //              ////Serial.print("Name is ");
    //              
                  for(unsigned int i =0; i<uuidSize; i++)
                  {
    //                 ////Serial.print(" Getting UUID at address of ");
    //                ////Serial.print(21 + selectedContactIndex* 15+i);
    //                ////Serial.println(char(read(21 + selectedContactIndex* 15+i)), HEX);
                    toUUID[i] = read(21 + selectedContactIndex*15 + i);
    //                
                  }
    //              ////Serial.print("NODE UUID IS ACTUALLY BEING SAVED BY ");
    //              printUUID(UUID, output);
    //              ////Serial.print("\n");
    //              ////Serial.print("\n TO UUID IS ");
    //              printUUID(toUUID, output);
                  ////Serial.print("\n");
    //              for(unsigned int i =0; i<nameSize; i++)
    //              {
    //                ////Serial.print(" Printing address of ");
    //                ////Serial.print(21 + selectedContactIndex* 15+i + 5);
    //                ////Serial.println(char(read(21 + selectedContactIndex* 15+i + 5)));
    //              }
                  ////Serial.print("Writing NODE UUID AT ADDRESS ");
                  char testUUID[uuidSize+1];
                  for(unsigned int i = 0; i<uuidSize; i++)
                  {
                    ////Serial.print(read(434) + i);
                    write(read(434) + i, UUID[i]);
                    testUUID[i]  = read(read(434) + i);
                    ////Serial.print("\n");
                  }
    //              ////Serial.print("\n PRINITNG UUID THAT HAS JUST BEEN SAVED\n");
    //              printUUID(testUUID, output);
                  ////Serial.print("\nSaving TO UUID at addresses \n");
                  for(unsigned int i = 0; i<uuidSize; i++)
                  {
                    ////Serial.print(read(434) + i+uuidSize);
                    write(read(434) + i+uuidSize, char(toUUID[i]));
                  }
                  
                  ////Serial.print("\n Reading TO UUID addresses \n");
                  for(unsigned int i = 0; i<uuidSize; i++)
                  {
                    ////Serial.print(read(434) + i+uuidSize);
                    testUUID[i] = read(read(434) + i+uuidSize);
                  }
                  ////Serial.print("\n");
                  printUUID(testUUID, output);
    
                  
    //              ////Serial.print("\nSaving Message at Address of ");
    //              ////Serial.print(read(434) + uuidSize + uuidSize);
    //              ////Serial.print(read(434) + uuidSize + uuidSize + 1);
    //              ////Serial.print("\n MESSAGES ARE \n");
    //              ////Serial.print(messageTop, BIN);
    //              ////Serial.print(messageBottom, BIN);
                  write(read(434) + uuidSize + uuidSize, messageTop);
                  ////Serial.print(read(read(434) + uuidSize + uuidSize), BIN);
                  write(read(434)+ uuidSize + uuidSize + 1, messageBottom);
                  ////Serial.print(read(read(434) + uuidSize + uuidSize+1), BIN);
                  write(read(434)+ uuidSize + uuidSize + 2, messageLength);
                  if(read(434) == 421)
                  {
                    write(434, 174);
                  }
                  else
                  {
                    write(434, read(434)+13);
                  }
                  write(173, read(173) + 1);
                lcd.clear();
                nextState = MENU;
                break;
              }
          
         
        }


      
        //WRITING MESSAGE BLOCK

       if(savedMessage == false) {
                                    char output[uuidSize+1];
                          char toUUID[uuidSize + 1];
//              ////Serial.print("SelectedContactIndex is ");
//              ////Serial.print(selectedContactIndex);
//              ////Serial.print("\n");
//              ////Serial.print(selectedContactIndex);
//              ////Serial.print("Name is ");
//              
              for(unsigned int i =0; i<uuidSize; i++)
              {
//                 ////Serial.print(" Getting UUID at address of ");
//                ////Serial.print(21 + selectedContactIndex* 15+i);
//                ////Serial.println(char(read(21 + selectedContactIndex* 15+i)), HEX);
                toUUID[i] = read(21 + selectedContactIndex*15 + i);
//                
              }
//              ////Serial.print("NODE UUID IS ACTUALLY BEING SAVED BY ");
//              printUUID(UUID, output);
//              ////Serial.print("\n");
//              ////Serial.print("\n TO UUID IS ");
//              printUUID(toUUID, output);
              ////Serial.print("\n");
//              for(unsigned int i =0; i<nameSize; i++)
//              {
//                ////Serial.print(" Printing address of ");
//                ////Serial.print(21 + selectedContactIndex* 15+i + 5);
//                ////Serial.println(char(read(21 + selectedContactIndex* 15+i + 5)));
//              }
              ////Serial.print("Writing NODE UUID AT ADDRESS ");
              char testUUID[uuidSize+1];
              for(unsigned int i = 0; i<uuidSize; i++)
              {
                ////Serial.print(read(434) + i);
                write(read(434) + i, UUID[i]);
                testUUID[i]  = read(read(434) + i);
                ////Serial.print("\n");
              }
//              ////Serial.print("\n PRINITNG UUID THAT HAS JUST BEEN SAVED\n");
//              printUUID(testUUID, output);
              ////Serial.print("\nSaving TO UUID at addresses \n");
              for(unsigned int i = 0; i<uuidSize; i++)
              {
                ////Serial.print(read(434) + i+uuidSize);
                write(read(434) + i+uuidSize, char(toUUID[i]));
              }
              
              ////Serial.print("\n Reading TO UUID addresses \n");
              for(unsigned int i = 0; i<uuidSize; i++)
              {
                ////Serial.print(read(434) + i+uuidSize);
                testUUID[i] = read(read(434) + i+uuidSize);
              }
              ////Serial.print("\n");
              printUUID(testUUID, output);

              
//              ////Serial.print("\nSaving Message at Address of ");
//              ////Serial.print(read(434) + uuidSize + uuidSize);
//              ////Serial.print(read(434) + uuidSize + uuidSize + 1);
//              ////Serial.print("\n MESSAGES ARE \n");
//              ////Serial.print(messageTop, BIN);
//              ////Serial.print(messageBottom, BIN);
              write(read(434) + uuidSize + uuidSize, messageTop);
              ////Serial.print(read(read(434) + uuidSize + uuidSize), BIN);
              write(read(434)+ uuidSize + uuidSize + 1, messageBottom);
              ////Serial.print(read(read(434) + uuidSize + uuidSize+1), BIN);
              write(read(434)+ uuidSize + uuidSize + 2, messageLength);
              if(read(434) == 421)
              {
                write(434, 174);
              }
              else
              {
                write(434, read(434)+13);
              }
              write(173, read(173) + 1);
            lcd.clear();
            nextState = MENU;
            
        }




      }
      break;
    case MESSAGE_FAILED:
    {
      if(previousState != MESSAGE_FAILED)
      {
                  timer = millis();
          while(millis() - timer < timeoutWait)
          {
            result = lcd.getButtonPress();
            doublePrintOnce("MESSAGE FAILED", 0, 0, NOCLEAR);
            if(result == UP)
            {
              lcd.clear();
              nextState = MENU;
              break;
            }
          }
          lcd.clear();
          nextState = MENU;
          break;
      }

    }
      break;
    case NEW_CONTACT:
      {
        // nextState = NEW_CONTACT_NAME;
        //delay(200);
        lcd.clear();
        //doublePrintOnce("Waiting...", 0, 0, NOCLEAR);
        if (memManager.getNumberContacts() >= 10)
        {
          static bool getOut  = false;
              if(memManager.getNumberContacts() >=10)
              {
                lcd.clear();
                nextState = LIST_FULL;
                ////Serial.print("GROING TO LIST_FULL\n");
                break;
              }
        }
        else
        {
          lcd.clear();
          nextState = NEW_CONTACT_NAME;
        }

      }
      break;
    case NEW_CONTACT_NAME:
      {
        debounceDelay = 150;
        doublePrintOnce("New Contact   [1", 0, 0, NOCLEAR);
        //GOT TO REDUCE DEBOUNCEDELAY SO THAT THE USER CAN SCROLL MORE QUICKLY
        static unsigned short letterScrollIndex = 0;
        static unsigned short letterAddedIndex = 0;
        // CHECK TO SEE WHAT THE MAX SIZE OF THE NAME IS!!!!!!!!
        static char* nameSoFar = "a          ";
        //        ////Serial.print("PRevious state is: ");
        //        ////Serial.print("\n");
        //        ////Serial.print(previousState);
        if (previousState == NEW_CONTACT)
        {
          letterAddedIndex = 0;
          letterScrollIndex = 0;
          for (unsigned int i = 0; i < nameSize + 1; i++)
          {
            if (i == 0)
            {
              nameSoFar[i] == 'a';
            }
            else
            {
              nameSoFar[i] = ' ';
            }
          }
        }
        switch (result)
        {
          case UP:
            {
              if (lettersWithNumbers[letterScrollIndex] == 'Z')
              {
                letterScrollIndex = 0;
              }
              else
              {
                letterScrollIndex++;
              }
              nameSoFar[letterAddedIndex] = lettersWithNumbers[letterScrollIndex];
            }
            break;
          case DOWN:
            {
              if (lettersWithNumbers[letterScrollIndex] == 'a')
              {
                letterScrollIndex = 51;
              }
              else
              {
                letterScrollIndex--;
              }
              nameSoFar[letterAddedIndex] = lettersWithNumbers[letterScrollIndex];
            }
            break;
          case RIGHT:
            {

              if (letterAddedIndex == nameSize)
              {
                letterAddedIndex = 0;
              }
              letterAddedIndex++;
              if (letterAddedIndex > 9)
              {
                letterAddedIndex--;
              }
              nameSoFar[letterAddedIndex] = lettersWithNumbers[letterScrollIndex];
              // ////Serial.print(letterAddedIndex);
              //////Serial.print("\n");
            }
            break;
          case LEFT:
            {

              if (letterAddedIndex != 0)
              {
                letterAddedIndex--;
              }
              if (letterAddedIndex == 0)
              {
                // letterAddedIndex = nameSize-1;
              }
              for (short i = letterAddedIndex + 1; i < 11; i++)
                nameSoFar[i] = ' ';
            }
            break;
          case SELECT:
            {
              lcd.clear();
              curNameSoFar = nameSoFar;
//              ////Serial.print("curNameSoFar IS NOW ");
//              ////Serial.print("\n");
//              ////Serial.print(curNameSoFar);
              //           for(unsigned int i = 0; i< nameSize; i++)
              //           {
              //            if(i == 0)
              //            {
              //              nameSoFar[i] = 'a';
              //            }
              //            else
              //            {
              //              nameSoFar[i] = ' ';
              //            }
              //           }

              nextState = NEW_CONTACT_UUID;

            }

            break;
          default:
            doublePrintOnce("Name: ", 1, 0, NOCLEAR);
            if ( letterAddedIndex == 0) {

              doublePrintOnce(nameSoFar, 1, int(sizeof("Name: ")) - 1, NOCLEAR);
              break;
            }
            for (unsigned int i = 0; i < letterAddedIndex; i++)
            {

              doublePrintOnce(nameSoFar, 1, int(sizeof("Name: ")) - 1, NOCLEAR);
              //lcd.print(nameSoFar[i]);

            }
            /*
              for(unsigned int i = 0; i<letterAddedIndex-1; i++)
              {
              {
                            doublePrintOnce(nameSoFar[letterAddedIndex], 1, letterAddedIndex, NOCLEAR);
              }
              //doublePrintOnce(nameSoFar[i], 1, letterAddedIndex, NOCLEAR);
              }
            */

            break;
        }
      }
      break;
    case NEW_CONTACT_UUID:
      {
        debounceDelay = 150;
        unsigned short uuidSizeForDisplay = 10;
        //GOT TO REDUCE DEBOUNCEDELAY SO THAT THE USER CAN SCROLL MORE QUICKLY
        static unsigned short letterScrollIndex = 26;
        static unsigned short letterAddedIndex = 0;
        // CHECK TO SEE WHAT THE MAX SIZE OF THE NAME IS!!!!!!!!
        static char uuid[] = "A         ";
        doublePrintOnce("New Contact   [2", 0, 0, NOCLEAR);
        if (previousState == NEW_CONTACT_NAME)
        {
          letterAddedIndex = 0;
          letterScrollIndex = 26;
          for (unsigned int i = 0; i < uuidSizeForDisplay + 1; i++)
          {
            if (i == 0)
            {
              uuid[i] == 'a';
            }
            else
            {
              uuid[i] = ' ';
            }
          }
        }
        switch (result)
        {
//            ////Serial.print("letter scross indez is ");
//            ////Serial.print("\n");
//            ////Serial.print(letterScrollIndex);
          case UP:
            {
              if (lettersWithNumbers[letterScrollIndex] == '9')
              {
                letterScrollIndex = 26;
              }
              else if (lettersWithNumbers[letterScrollIndex] == 'F')
              {
                letterScrollIndex = 52;
              }
              else
              {
                letterScrollIndex++;
              }
              uuid[letterAddedIndex] = lettersWithNumbers[letterScrollIndex];
            }
            break;
          case DOWN:
            {
//              ////Serial.print("letter scross indez is ");
//              ////Serial.print("\n");
//              ////Serial.print(letterScrollIndex);
              if (lettersWithNumbers[letterScrollIndex] == 'A')
              {
                letterScrollIndex = 61;
              }
              else if (lettersWithNumbers[letterScrollIndex] == '0')
              {
                letterScrollIndex = 31;
              }
              else
              {
                letterScrollIndex--;
              }
              uuid[letterAddedIndex] = lettersWithNumbers[letterScrollIndex];
            }
            break;
          case RIGHT:
            {


              if (letterAddedIndex >= uuidSizeForDisplay - 1)
              {
                letterAddedIndex = uuidSizeForDisplay - 1;
              }
              else
              {
                letterAddedIndex++;
              }
              uuid[letterAddedIndex] = lettersWithNumbers[letterScrollIndex];
              // ////Serial.print(letterAddedIndex);
              //////Serial.print("\n");
            }
            break;
          case LEFT:
            {

              if (letterAddedIndex != 0)
              {
                letterAddedIndex--;
              }
              if (letterAddedIndex == 0)
              {
                // letterAddedIndex = nameSize-1;
              }
              for (short i = letterAddedIndex + 1; i < 11; i++)
                uuid[i] = ' ';
            }
            break;
          case SELECT:
            {

              char outputUUID[uuidSize + 1];
              char printer[uuidSize+1];
              for(unsigned int i = letterAddedIndex+1; i<nameSize; i++)
              {
                uuid[i] = '0';
              }
              convertToUUID(uuid, outputUUID);
              printUUID(outputUUID, printer);
              lcd.clear();
              for(unsigned int i =0; i<uuidSize; i++)
              {

                ////Serial.print(" CURRENT UUID IS \n");
                
                curUUID[i] = outputUUID[i];
                ////Serial.print(curUUID[i]);
              }
              debounceDelay = 1000;
              nextState = CONTACT_ADDED;
            }
            break;
          default:
            doublePrintOnce("UUID: ", 1, 0, NOCLEAR);

            if ( letterAddedIndex == 0) {
              delay(50);
              doublePrintOnce(uuid, 1, int(sizeof("UUID: ")) - 1, CLEAR);
              break;
            }
            for (unsigned int i = 0; i < letterAddedIndex; i++)
            {
              doublePrintOnce(uuid, 1, int(sizeof("UUID: ")) - 1, NOCLEAR);
              //lcd.print(uuid[i]);

            }
            /*
              for(unsigned int i = 0; i<letterAddedIndex-1; i++)
              {
              {
                            doublePrintOnce(uuid[letterAddedIndex], 1, letterAddedIndex, NOCLEAR);
              }
              //doublePrintOnce(uuid[i], 1, letterAddedIndex, NOCLEAR);
              }
            */

            break;
        }
      }
      break;
    case CONTACT_ADDED:
      {
                static long timerStatic = millis();
                char output[nameSize + 1];
                Contact h = Contact(curUUID, curNameSoFar);
                write(20,read(20) + 1);
        if(previousState == NEW_CONTACT_UUID)
        {
          timer = millis();
          
        }
        while(millis() - timer < timeoutWait)
        {
          doublePrintOnce("Contact Added!",0,0,NOCLEAR);
//          ////Serial.print("\n STILL WAITING");
          result = lcd.getButtonPress();
          if(result == UP)
          {
                
              if ((memManager.saveContact(h)) == false)
              {
                  curNameSoFar = "           ";
                curUUID = "            ";
                lcd.clear();
                nextState = LIST_FULL;
                break;
              }
              else
              {
                ////Serial.print("\n SAVING CONTACT!");
                memManager.saveContact(h);
              }
                curNameSoFar = "           ";
                curUUID = "            ";
            lcd.clear();
            nextState = MENU;
           break; 
          }
        }
          if ((memManager.saveContact(h)) == false)
              {
                  curNameSoFar = "           ";
                curUUID = "            ";
                lcd.clear();
                nextState = LIST_FULL;
                break;
              }
              else
              {
                ////Serial.print("\n SAVING CONTACT!");
                memManager.saveContact(h);
              }
                curNameSoFar = "           ";
                curUUID = "            ";
            lcd.clear();
            nextState = MENU;
           break;   
      }
      break;
    case LIST_FULL:
      {
        
        if(previousState != LIST_FULL)
        {
          //Serial.print("LIST FULL STATE\n");
          timer = millis();
          while(millis() - timer < timeoutWait)
          {
            result = lcd.getButtonPress();
            doublePrintOnce("Contact List ", 0, 0, NOCLEAR);
            doublePrintOnce("is full!", 1, 0, NOCLEAR);
            if(result == UP)
            {
                            curNameSoFar = "a          ";
              curUUID = "a           ";
              lcd.clear();
              nextState = MENU;
              break;
            }
          }
          curNameSoFar = "a          ";
          curUUID = "a           ";
          lcd.clear();
          nextState = MENU;
          break;
        }
//        switch (result)
//        {
//          case UP:
//            {
//              curNameSoFar = "a          ";
//              curUUID = "a           ";
//              lcd.clear();
//              nextState = MENU;
//            }
//        }
      }
      break;
    case ABOUT_ME:
      {
        if(previousState != ABOUT_ME)
        {
           char output[uuidSize + 1];
          printUUID(UUID, output);
          doublePrintOnce("Name: ", 0, 0, NOCLEAR);
          doublePrintOnce(nodeName, 0, short(sizeof("Name: "))-1, NOCLEAR);
          doublePrintOnce("UUID: ", 1, 0, NOCLEAR);
          doublePrintOnce(output,1, short(sizeof("UUID: "))-1, NOCLEAR); 
        }

        switch (result)
        {
          case UP:
            {
                           lcd.clear();
              nextState = MENU;
 
            }
            break;
                  case SELECT:
            {
              lcd.clear();
              nextState = SETUP;
            }
            break;
        }

      }
      break;
  }
  previousState = currentState;
  currentState = nextState;


  ////Serial.print(analogRead(0));
  ////Serial.print(analogRead(0));
  // put your main code here, to run repeatedly:

}
