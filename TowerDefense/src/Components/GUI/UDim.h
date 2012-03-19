#pragma once
#include "../../vmath.h"

namespace Edge
{
   enum Enum
   {
      None=0, Top=1, Bottom=2, Left=4, Right=8, 
      CentreX=16, CentreY=32,
      TopLeft      = (1  | 4), TopRight     = (1 | 8),
      BottomLeft   = (2  | 4), BottomRight  = (2 | 8),
      CentreLeft   = (32 | 4), CentreRight  = (32 | 8),
      CentreTop    = (16 | 1), CentreBottom = (16 | 2),
      CentreCentre = (16 | 32)
   };
}

/*
 * UDim represents a screenspace coordinate
 * I can be specified relative to any screen edge
 * and offset by an absolute number of pixels
 * Eg rx=0.5 is the centre of the screen
 * If an object has a size then you can also specify the edge
 * to align.
 */
class UDim
{
private:
   Edge::Enum mObjectEdge;
   Vector2f mRelative;
   Vector2f mAbsolute;
public:
   UDim();
   UDim(Vector2f relative, Vector2f absolute);
   UDim(Vector2f relative, Vector2f absolute, Edge::Enum object_edge);

   Vector2f ToScreen(Vector2f screen_size);
   Vector2f ToScreen(Vector2f screen_size, Vector2f object_size);

   bool operator== (const UDim& c2) const;
};
