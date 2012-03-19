#pragma once
#include <vector>
#include <algorithm>

template <class T> class MessageHub;
class MessageRoot;

/*
 * Subscriber allows a subscription to a message type to be scoped
 * and auto unsubscribe when the subscriber passes out of scope
 * Each call to GetMessages will return a fresh list and should only
 * be called once per Tick
 */
template <class T> class Subscriber
{
public:
   void Initialise(MessageRoot* msg);

   Subscriber()
      : mMsg(0)
   {
   }

   ~Subscriber();

   std::vector<T> GetMessages()
   {
      std::vector<T> mcopy = mMessages;
      mMessages.clear();
      return mcopy;
   }

   friend class MessageHub<T>;
private:
   std::vector<T> mMessages;
   MessageRoot* mMsg;

   void AddMessage(T message)
   {
      mMessages.push_back(message);
   }
};

class HubBase
{
public:
   virtual void Distribute() = 0;
};

/*
 * Messagehub is responsible for distributing messages to 
 * subscribers.
 */
template <class T> class MessageHub : public HubBase
{
public:
   void Subscribe(Subscriber<T>* subscriber)
   {
      mSubscribers.push_back(subscriber);
   }

   void Unsubscribe(Subscriber<T>* subscriber)
   {
      mSubscribers.erase(std::remove(mSubscribers.begin(), mSubscribers.end(), subscriber), mSubscribers.end());
   }

   /*
    * Distributes all messages queued up since last call to all subscribers
    */
   virtual void Distribute()
   {
      /* Learn something new every day! It is a syntax error in strict C++ to use nested class of a generic type
         without qualifying it with typename */

      for(typename std::vector<Subscriber<T>*>::iterator it = mSubscribers.begin(); it != mSubscribers.end(); ++it) //Fuck you GCC typename my ass
      {
         for(typename std::vector<T>::iterator it_m = mMessages.begin(); it_m != mMessages.end(); ++it_m)
         {
            (*it)->AddMessage(*it_m);
         }
      }
      mMessages.clear();
   }

   void Broadcast(T message)
   {
      mMessages.push_back(message);
   }

   friend class MessageRoot;
private:
   MessageHub(){}
   ~MessageHub(){}

   std::vector<T> mMessages;
   std::vector<Subscriber<T>*> mSubscribers;
};

/*
 * MessageRoot 
 */
class MessageRoot
{
public:
   template <class T> MessageHub<T>& GetHub()
   {
      static MessageHub<T> instance;
      static bool first = true;

      //Store hub to iterate over
      if(first)
      {
         mHubs.push_back(&instance);
         first = false;
      }

      return instance;
   }

   void Distribute()
   {
      for(std::vector<HubBase*>::iterator it = mHubs.begin(); it != mHubs.end(); ++it)
      {
         (*it)->Distribute();
      }
   }
private:
   std::vector<HubBase*> mHubs;
};


//Implementation of Subscriber now MessageRoot and MessageHub are known
template <class T> void Subscriber<T>::Initialise(MessageRoot* msg)
{ 
   if(mMsg)
      mMsg->GetHub<T>().Unsubscribe(this);
   mMsg = msg;
   mMsg->GetHub<T>().Subscribe(this);
}

template <class T> Subscriber<T>::~Subscriber()
{
   if(mMsg)
      mMsg->GetHub<T>().Unsubscribe(this);
   mMsg = 0;
}
