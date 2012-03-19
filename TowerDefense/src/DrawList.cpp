#include "Drawlist.h"
#include "Components/Component.h"
#include <GLES2/gl2.h>
#include "Log.h"
#include "CheckMacros.h"

void DrawList::AddToLayer(DrawLayer::Enum layer, Component* component)
{
   mLayers[layer].push_back(component);
}

void DrawList::RemoveFromLayer(DrawLayer::Enum layer, Component* component)
{
   std::vector<Component*>& items = mLayers[layer];
   items.erase(std::remove(items.begin(), items.end(), component), items.end());
}

void DrawList::ClearAll()
{
   for(std::map<DrawLayer::Enum, std::vector<Component*> >::iterator it = mLayers.begin(); it != mLayers.end(); ++it)
   {
      it->second.clear();
   }
}

void DrawList::DrawLayer(DrawLayer::Enum layer, TickParameters& tp)
{
   //Setup layer state
   switch(layer)
   {
   case DrawLayer::Game:
      GL_CHECK(glEnable(GL_DEPTH_TEST));
      break;
   case DrawLayer::GUI:
   case DrawLayer::Fade:
      GL_CHECK(glDisable(GL_DEPTH_TEST));
      break;
   default:
      Log::Error("DrawList.cpp", "Unknown DrawLayer %d", layer);
      break;
   }

   //Draw the layer
   for(std::vector<Component*>::iterator it = mLayers[layer].begin(); it != mLayers[layer].end(); ++it)
   {
      (*it)->Draw(tp);
   }
}
