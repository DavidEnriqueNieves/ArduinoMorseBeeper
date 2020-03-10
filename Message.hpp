#ifndef MessageH
#define MessageH

byte read(unsigned int uiAddress)
{
  //WHILE THE EEPE IS NOT 0 since the write must be finished
  //Last write function must have set EEPE to 0
  while(EECR & (1<<EEPE));
  EEAR = uiAddress;
  EECR = EECR|(1<<EERE);
  return EEDR;
}
void write(unsigned int uiAddress, unsigned char data)
{
  while(EECR & (1<<EEPE));
  EEAR = uiAddress;
  EEDR = data;
  //???  DO THEY WANT ucDATA OR just DATA
  EECR = EECR|(1<<EEMPE);
  EECR = EECR | (1<<EEPE);
}

class Message {
  public:
    Message();
    Message(unsigned char* from, unsigned char* to, unsigned short payload, unsigned char length);
    Message(unsigned char* from, unsigned char* to, char const* message);
    void setLength(unsigned char length);
    void setTo(unsigned char* to);
    void setFrom(unsigned char* from);
    void setPayload(unsigned short payload);
    unsigned char getLength();
    unsigned char* getTo();
    unsigned char* getFrom();
    unsigned short getPayload();
    char* getPayloadString();
    friend class Memory;

  protected:
    unsigned short stringToPayload(char const* message);
    char* payloadToString(unsigned short payload, unsigned char length);

  private:
  static unsigned int numMessages;
  unsigned char* toAddress;
  unsigned char* fromAddress;
  unsigned short payload;
  const unsigned short messageCounterOffset  = 20;
    // ...
    // ...
    // ...
};

  static unsigned int Message::numMessages = 0;

  
   Message::Message()
   {
    Message::numMessages+=1;
    write(messageCounterOffset, Message::numMessages);
   }
    Message::Message(unsigned char* from, unsigned char* to, unsigned short payload, unsigned char length)
    {
      Message::numMessages++;
      write(messageCounterOffset, Message::numMessages);

      this->fromAddress = from;
      this->toAddress = to;
      this->payload = payload;

    }
    Message::Message(unsigned char* from, unsigned char* to, char const* message)
    {
      Message::numMessages++;
      write(messageCounterOffset, Message::numMessages);
    }
  void Message::setTo(unsigned char* to)
    {
      toAddress = to;
    }
    void Message::setFrom(unsigned char* from)
    {
      fromAddress = from;
    }
    
    //void Message::setPayload(unsigned short payload);
    //unsigned char Message::getLength();
    
    unsigned char* Message::getTo()
    {
      return toAddress;
    }
    unsigned char* Message::getFrom()
    {
      return fromAddress;
    }
    unsigned short Message::getPayload();
    char* Message::getPayloadString();
    unsigned short Message::stringToPayload(char const* message);
    char* Message::payloadToString(unsigned short payload, unsigned char length);
  
  




  

#endif
