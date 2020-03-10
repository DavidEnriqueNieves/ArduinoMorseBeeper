
#ifndef MemoryH
#define MemoryH
#include "Contact.hpp"
#include "Message.hpp"


const unsigned short maxNumMessages = 20;
const unsigned short maxNumContacts = 10;
const unsigned short messageSize = 16;
const unsigned short buzzerPin = 3;
const unsigned short nameSize = 10;
const unsigned short uuidSize = 5;
//  int Contact::numContacts = 0;
  
  

class Memory {
  public:
    Memory();
    Memory(Contact node);
    unsigned char* getNodeUUID();
    char* getNodeName();
    unsigned short getNumberContacts();
    unsigned short getNumberMessages();
    Contact getContact(unsigned short index);
    Message getMessage(unsigned short index);
    bool saveContact(Contact contact);
    void saveMessage(Message message);
    void saveNodeInformation(Contact contact);
    // Add as you see fit
    friend class Message;
    friend class Contact;
    bool hasSchema();
    void setSchema();
    void clearMessages();
    void clearContacts();
    unsigned short getMessagePointerOffset();
    
    short Memory::foundUUID(byte uuid[uuidSize+1]);
    short getUUIDName(char uuid[uuidSize + 1]);
    // Add as you see fit

  private:
//    const unsigned short MAX_CONTACTS = 10;
//    const unsigned short MAX_MESSAGES = 20;
    // Add as you see fit

};

inline void printUUID(char uuid[], char output[11])
{
  int counter  = 0 ;
  for (unsigned int  i = 0; i < 5; i++)
  {
    //  std::cout << std::bitset<4>((int(uuid[i])>>4)&15)<<" ";
    if (((int(uuid[i]) >> 4) & 15) > 9)
    {

      output[counter] = (char(((int(uuid[i]) >> 4) & 15) + 55));
      //  //Serial.print(char(((int(uuid[i])>>4)&15) + 55));
      counter++;
    }
    else
    {
      output[counter] = char(((int(uuid[i]) >> 4) & 15) + 48);
      //   //Serial.print((int(uuid[i])>>4)&15);
      counter++;
    }
    if (((int(uuid[i]) & 15)) > 9)
    {
      output[counter] = (char(((int(uuid[i]) & 15)) + 55));
      //   //Serial.print(char(((int(uuid[i])&15))+ 55));
      counter++;
    }
    else
    {
      output[counter] = char((((int(uuid[i]) & 15))) + 48);
      //  //Serial.print(((int(uuid[i])&15)));
      counter++;
    }

    // std::cout << std::bitset<4>((int(uuid[i])&15));
    // std::cout << "\n";
  }
  //Serial.print("\n");
  for (unsigned int i = 0; i < 10; i++)
  {
    //Serial.print(output[i]);
  }

}
void convertToUUID(char input[], char output[])
{
//  for(unsigned int i = 0; i<nameSize; i++)
//  {
//     //Serial.print(input[i]);
//     //Serial.print("\n");
//  }
    //Serial.print("INPUT IS  \n");
  char temp[uuidSize + 1];
  for (unsigned int i = 0; i < 10; i++)
  {
    //Serial.println(char(input[i]));
//    //Serial.print(temp[i], HEX);
//      std::cout << ("Temp is ... ");
//        std::cout << std::bitset<8>(temp);
//        std::cout << ("\n");
//       std::cout << " i is "<< i/2<<"\n";
  
//   std::cout << output[i/2]<<"\n";
  }
  //Serial.print("\n NEW TEMP IS \n");
  for(unsigned int i = 0; i< 10; i+=2)
  {
    if ((int(input[i]) >= 65) && (int(input[i]) <= 70))
    {
      temp[i] = (int(input[i]) - 55) * 16;
    }
    else if ((int(input[i]) >= 48) && (int(input[i]) <= 57))
    {
      temp[i] = (int(input[i]) - 48) * 16;
    }
    if ((int(input[i + 1]) >= 65) && (int(input[i + 1]) <= 70))
    {
      temp[i] += (int(input[i + 1]) - 55);
    }
    else if ((int(input[i + 1]) >= 48) && (int(input[i + 1]) <= 57))
    {
      temp[i] += (int(input[i + 1]) - 48);

    }
    output[i/2] = temp[i];
    //Serial.print(temp[i], HEX);
  }

  
}


bool Memory::hasSchema()
{
  //Serial.print("HAS SCHEMA FUNCTION");
  //Serial.print("\n");
    bool alreadySetup = true;
    if ((read(0) != 0xC0) || (read(1) != 0xFF) || (read(2) != 0xEE))
    {
      //Serial.print("COFFEE FLAG NOT SET");
      //Serial.print("\n");
      alreadySetup = false;
    }
    else if ((read(18) != 0xFA) || (read(19) != 0xCE))
    {
      //Serial.print("FACE FLAG NOT SET");
      //Serial.print("\n");
      alreadySetup = false;
    }
    else if ((read(171) != 0xCA) || (read(172) != 0x11))
    {
      //Serial.print("CA11 FLAG NOT SET");
      //Serial.print("\n");
      alreadySetup = false;
    }
    else if ((read(173) > maxNumMessages) || (read(20) > maxNumContacts))
    {
      //Serial.print("maxNumMessages or maxNumContacts exceeds number");
      //Serial.print("\n");
      //Serial.print("NUMBER OF MESSAGES ");
      //Serial.print(read(173));
      //Serial.print("\n");
      //Serial.print("NUMBER OF CONTACTS ");
      //Serial.print(read(20));
      //Serial.print("\n");
      alreadySetup = false;
    }

    return alreadySetup;

  
}


void Memory::setSchema()
{
  //Serial.print("\n");
  //Serial.print("WRITING SCHEMA");
      write(0, 0xC0);
    write(1, 0xFF);
    write(2, 0xEE);
write(18, 0xFA);
write(19, 0xCE);
write(171, 0xCA);
write(172, 0x11);
this->clearMessages();
this->clearContacts();
write(434, 174);
}


void Memory::clearMessages()
{
  
        
        //Serial.print("ERASING MESSAGES--------------------------------");
        for(unsigned int i = 0; i< maxNumMessages; i++)
        {
          for(unsigned int p = 0; p<(uuidSize + uuidSize + 3); p++)
          {
//            //Serial.print("Resetting at memory offset of ");
//            //Serial.print(174 + i*13 + p);
            write(174+ i*13 + p, 0);
//            //Serial.print(" | ");
//            //Serial.print(" COntains: ");
//            //Serial.print(read(174+i*13+p));
//            //Serial.print("\n");
//            //Serial.print("Resetting MessageLength");
            
          }
        }
        write(173,0);
}

void Memory::clearContacts()
{
//  
//        //Serial.print("\n");
//        //Serial.print("SETUP");
//        //Serial.print("\n");
//        
        for (unsigned int u = 0; u < maxNumContacts + 1; u++)
        {
          unsigned short memoryOffset = 20 + (u - 1) * 15 + 1;
//          //Serial.print("RESETTING AT ADDRESS");
//          //Serial.print("\n");
//          //Serial.print(memoryOffset);
//          //Serial.print("\n");
          //RESETTING UUIDS
          for (unsigned int i = 0; i < uuidSize; i++)
          {
//            //Serial.print("RESETTING  at memory OFFSET of ");
//            //Serial.print(memoryOffset + i);
//            //Serial.print("\n");
//            //Serial.print("VALUE IS | ");
            write(memoryOffset + i, 0);
//            //Serial.print(read(memoryOffset + i));

          }
          //ReSETTING THE CONTACT NAMES
          for (unsigned int i = 0; i < nameSize; i++)
          {
            write(memoryOffset + i + uuidSize, ' ');
          }
          
        }
        write(20, 0);
}


Memory::Memory()
{
  
}
 Memory::Memory(Contact node)
  {
    
  }
  unsigned char* Memory::getNodeUUID()
  {
    unsigned char* UUID;
    
    for(unsigned int i = 0; i<5; i++)
    {
      UUID[i] = read(3 + i);
    }
    
    return UUID;
  }
  
     char* Memory::getNodeName()
  {
     char* nameR;
    
    for(unsigned int i = 0; i<5; i++)
    {
      nameR[i] = read(8 + i);
    }
    
    return nameR;
  }
    unsigned short Memory::getNumberContacts()
  {
     ////Serial.print(Contact::numContacts);
    return read(20);
  }
    unsigned short Memory::getNumberMessages()
  {
    ////Serial.print(Message::numMessages);
    return read(173);
  }
    Contact Memory::getContact(unsigned short index)
  {
    
    return Contact();
  }
    Message Memory::getMessage(unsigned short index)
  {
    return Message();
  }
    bool Memory::saveContact(Contact contact)
  {
    
    if((20 + (read(20) - 1) * 15 + 1)> 156)
    {
      //Serial.print("CURRENT NUMBER OF CONTACTS IS \n");
      //Serial.print(read(20));
     return false; 
    }
    else
    {
//      //Serial.print("COUNTER OFFSET IS CURRENTLY");
//      //Serial.print("\n");
//      //Serial.print(read(20));
//      //Serial.print("\n");
      unsigned short memoryOffset = 20 + ((read(20)) - 1) * 15 + 1;
//      //Serial.print("CONTACT GETTING SAVED AT ADDRESS");
//      //Serial.print("\n");
//      //Serial.print(memoryOffset);
            //Serial.print("\n");
      unsigned char* uuid = contact.getUUID();
      unsigned char* contName = contact.getName();
      for(unsigned int i = 0; i< uuidSize; i++)
      {
//      //Serial.print("Saved at memory OFFSET of ");
//        //Serial.print(memoryOffset+i);
//        //Serial.print("\n");
        write(memoryOffset+i,reinterpret_cast<unsigned char>(uuid[i]));
      }
      for(unsigned int i = 0; i< nameSize; i++)
      {
        write(memoryOffset+i + uuidSize,reinterpret_cast<unsigned char>(contName[i]));
      }
      return true;
    }
  }
     void Memory::saveMessage(Message message)
  {
    unsigned short memoryOffset = read(434);
    char senderUUID[5]; 
    convertToUUID(message.getFrom(), senderUUID);
    char receiverUUID[5]; 
    convertToUUID(message.getTo(), receiverUUID);
    char output[11];
//    //Serial.print("Sender UUID IS ");
//    //printUUID(senderUUID, 
//    //Serial.print("\n");
//    //Serial.print("Receiver UUID IS ");
//    //Serial.print(receiverUUID); 
    
  }
   void Memory::saveNodeInformation(Contact contact)
  {
  }


    short Memory::foundUUID(byte uuid[uuidSize+1])
    {
      short nameOffset = 1;
      byte output[uuidSize + 1];
//      Serial.print("\n UUID BEING SEARCHED FOR IS ");
//      for(unsigned int i =0; i< uuidSize; i++)
//      {
//        Serial.println(uuid[i], HEX);
//      }
      
      short memoryOffset = 21;
      bool topFound = false;
//      Serial.println("NUM OF CONTACTS");
//      Serial.println(read(20));
//      Serial.print(read(20) + 21);
//      
         for(unsigned int i =0; i<read(20); i++)
         {
//          Serial.print("Looking for UUID at memory offset of\n");
//          Serial.print(memoryOffset + i*(uuidSize + nameSize));
          //Serial.print("\n");
           bool found  = true;         
           char temp[uuidSize  +1];
           Serial.println("CONTACT UUID IS\n");
           for(unsigned int p = 0; p<uuidSize; p++)
           {
  //            temp[i] = byte(read(memoryOffset + i*(uuidSize + nameSize) + p));
  //            Serial.print(read(memoryOffset + i*(uuidSize + nameSize) + p), HEX);
//              Serial.print("  |  ");
//              Serial.print(uuid[p], HEX); 
               if(byte(read(memoryOffset + i*(uuidSize + nameSize) + p)) != byte(uuid[p]))
              {
//                       Serial.print("FAAALSE\n");
//                       Serial.print("---------------------\n");
//                Serial.print((read(memoryOffset + i*15 + p)), HEX);
//                Serial.print("\n");
//                Serial.print(uuid[p], HEX);
//                Serial.print("\n");
//                Serial.print("---------------------\n");
                found = false;
              }
                 if(found == true)
                 {
//                  Serial.print("Found at Memory Offset of ");
//                  Serial.print(memoryOffset + i*(uuidSize + nameSize) + uuidSize);
                  nameOffset = memoryOffset + i*(uuidSize + nameSize) + uuidSize;
                  return nameOffset;
                  break;
                 }
                 else if(i == this->getNumberContacts() - 1)
                 {
//                  Serial.println("NOT FOIUND");
                  //Serial.print("\nReturning -1!!\n");
                  return -1;
                 }
              
              //Serial.print("\n");
           }
       
         }
    }


//
//
//
//  
//{
//                  if (sent == true)
//                {
//                  //look for receiver UUID's name 
//                  //if not found, then say Anonymous
//                  for(unsigned int i =0; i<memManager.getNumberContacts(); i++)
//                  {
//                    
//                    for(unsigned int p = 0; p<uuidSize; p++)
//                    {
//                    
//                    
//                    }
//                  }
//                  
//                }
//                else
//                {
//                  //look for sender UUID's name in memory 
//                  //if not found, then say Anonymous
//                    for(unsigned int i =0; i<memManager.getNumberContacts(); i++)
//                    {
//                      for(unsigned int p = 0; p<uuidSize; p++)
//                      {
//                        
//                      }
//                    }
//                      
//                  } 
//}
//
//


  

#endif
