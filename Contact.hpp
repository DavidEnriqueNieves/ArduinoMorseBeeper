#define MAXNUMCONTACTS 10
#ifndef ContactH
#define ContactH
#include "Memory.hpp"
#include "Message.hpp"



class Contact {
  public:
  
    Contact()
    {
    numContacts+=1;
      if(read(20) <= MAXNUMCONTACTS)
      {
        write(20, read(20)+1);
      }
    }
    Contact(unsigned char* givenUUID, char const* givenName)
    {
      UUID = givenUUID;
      name = givenName;
      numContacts+=1;
      if(read(20) <= MAXNUMCONTACTS)
      {
        write(20, read(20)+1);
      }
    }
    void setUUID(unsigned char* givenUUID)
    {
      UUID = givenUUID;
    }
    void setName(char const* givenName)
    {
      name  = givenName;
    }
    void Contact::setName(char givenName)
    {
      name = givenName;
    }
    unsigned char* getUUID()
    {
      return UUID;
    }
    char* getName()
    {
      return name;
    }
    friend class Memory;
  private:
    static int numContacts;
    char* name;
    char* UUID;
    // ...
    // ...
    // ...
};
 int Contact::numContacts = 0;

#endif

 
