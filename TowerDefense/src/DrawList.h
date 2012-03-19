#pragma once
#include <vector>
#include <map>

class Component;
struct TickParameters;

namespace DrawLayer
{
   enum Enum
   {
      Game, GUI, Fade
   };
}

/*
 * Drawlist contains a sequence of layers that are drawn
 * one after another. They may be separated by state changes,
 * eg GUI layer stuff will be separated from GameLayer stuff
 * by a disabling of ztesting
 */
class DrawList
{
private:
   std::map<DrawLayer::Enum, std::vector<Component*> > mLayers;

public:
   void AddToLayer(DrawLayer::Enum layer, Component* component);
   void RemoveFromLayer(DrawLayer::Enum layer, Component* component);

   void ClearAll();
   void DrawLayer(DrawLayer::Enum layer, TickParameters& tp);
};
