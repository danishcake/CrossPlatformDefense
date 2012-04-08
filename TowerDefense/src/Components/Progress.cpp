#include "Progress.h"

Progress::Progress(std::string level_name) :
   mLevelName(level_name), mWaveID(0)
{
}

std::string Progress::GetLevelName()
{
   return mLevelName;
}

int Progress::GetWaveID()
{
return mWaveID;
}

void Progress::AdvanceToNextWave()
{
   mWaveID++;
}

void Progress::Tick(TickParameters& tp)
{
}
