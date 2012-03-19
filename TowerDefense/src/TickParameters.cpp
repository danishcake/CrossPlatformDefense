#include "TickParameters.h"
#include "Camera.h"

TickParameters::TickParameters(IFile* file)
{
   timespan = 0.0166666666f;
   this->file = file;
   camera = new Camera();
}