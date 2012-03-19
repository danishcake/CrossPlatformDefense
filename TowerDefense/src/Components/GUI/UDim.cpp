#include "UDim.h"

UDim::UDim() :
   mObjectEdge(Edge::None)
{
}

UDim::UDim(Vector2f relative, Vector2f absolute)
 : mRelative(relative), mAbsolute(absolute),
   mObjectEdge(Edge::None)
{
}

UDim::UDim(Vector2f relative, Vector2f absolute, Edge::Enum object_edge)
 : mRelative(relative), mAbsolute(absolute),
   mObjectEdge(object_edge)
{
}

Vector2f UDim::ToScreen(Vector2f screen_size)
{
   return screen_size * mRelative + mAbsolute;
}

Vector2f UDim::ToScreen(Vector2f screen_size, Vector2f object_size)
{
   Vector2f position = ToScreen(screen_size);

   float x = 0;
   float y = 0;
   
   if(mObjectEdge & Edge::Left)
      x = 0;
   if(mObjectEdge & Edge::Right)
      x = object_size.x;
   if(mObjectEdge & Edge::CentreX)
      x = object_size.x / 2;

   if(mObjectEdge & Edge::Top)
      y = 0;
   if(mObjectEdge & Edge::Bottom)
      y = object_size.y;
   if(mObjectEdge & Edge::CentreY)
      y = object_size.y / 2;   
      
   return position - Vector2f(x, y);
}

bool UDim::operator== (const UDim& d2) const
{
   return (d2.mAbsolute == mAbsolute &&
           d2.mObjectEdge == mObjectEdge &&
           d2.mRelative == mRelative);
}