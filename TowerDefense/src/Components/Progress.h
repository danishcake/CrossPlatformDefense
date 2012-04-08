#pragma once

#include "Component.h"

/*
 * Progress Tracks the wave ID, score and other related areas
 */
class Progress : public Component
{
public:
   Progress(std::string level_name);

   std::string GetLevelName();

   int GetWaveID();
   void AdvanceToNextWave();

   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {}

private:
   std::string mLevelName;
   int mWaveID;
};