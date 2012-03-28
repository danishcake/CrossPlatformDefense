#include "BuildGS.h"
#include "../../GameObject.h"
#include "../GUI/Actions/SignalAction.h"
#include "../Graphics/WorldDrawer.h"
#include "../Graphics/PathVisualiser.h"
#include "../Graphics/CubeDrawer.h"
#include "../Graphics/CursorDrawer.h"
#include "../Position.h"
#include "../Navigator.h"
#include "../WorldHandle.h"
#include "../CursorEventReceiver.h"
#include "../GUI/ControlArea.h"
#include "../GUI/ControlOutline.h"
#include "../GUI/ControlEventReceiver.h"
#include "../GUI/ControlEventDistributor.h"
#include "../GUI/ControlText.h"
#include "../GUI/Actions/CameraRotateAction.h"
#include "../GUI/Actions/CursorPositioningAction.h"
#include "../StateListener.h"
#include "../../WorldBlocks.h"



BuildGS::BuildGS(WorldBlocks* world)
   : mFirst(true), mBlocks(world)
{
   mFromMenu = world == 0;
}

void BuildGS::Tick(TickParameters& tp)
{
   if(mFirst)
   {
      SpawnMenuObjects(tp);
      mFirst = false;
   }
}

void BuildGS::Teardown(TickParameters& tp)
{
   
}

void BuildGS::SpawnMenuObjects(TickParameters& tp)
{
   //Load the world
   if(mBlocks == 0)
   {
      mBlocks = WorldBlocks::LoadFromPNG(tp, "heightmaps/map2.png");

      //Spawn the world
      GameObject* world = new GameObject();
      world->AddComponent(new WorldDrawer(mBlocks), tp);
      world->AddComponent(new StateListener(GameStates::Build, GameStates::Defend, true), tp);
      tp.Spawn(world);
   }

   //Spawn GUI controls
   if(mFromMenu)
   {
      //Spawn cursor
      GameObject* cursor = new GameObject();
      cursor->AddComponent(new Position(), tp);
      cursor->AddComponent(new CursorDrawer(), tp);
      cursor->AddComponent(new CursorEventReceiver(), tp);
      tp.Spawn(cursor);

      //Spawn drag area for Camera manipulation, tap detection (with callback)
      GameObject* drag_area = new GameObject();
      drag_area->AddComponent(new ControlArea(UDim(Vector2f(0.0f, 0.0f), Vector2f( 0.0f,  0.0f), Edge::TopLeft),
                                              UDim(Vector2f(1.0f, 0.9f), Vector2f( 0.0f, -20.0f))), tp);
      drag_area->AddComponent(new ControlEventReceiver(), tp);
      drag_area->AddComponent(new CameraRotateAction(CameraAction::TurnAxis), tp);
      drag_area->AddComponent(new CursorPositioningAction(mBlocks), tp);
      drag_area->AddComponent(new StateListener(GameStates::Build, GameStates::Defend, true), tp);
      tp.Spawn(drag_area);
      
      //Spawn camera controls
      GameObject* gui_right = new GameObject();
      gui_right->AddComponent(new ControlArea(UDim(Vector2f(1.0f, 1.0f), Vector2f(-10.0f, -10.0f), Edge::BottomRight),
                                              UDim(Vector2f(0.20f, 0.1f), Vector2f( 0.0f,  10.0f))), tp);
      gui_right->AddComponent(new ControlEventReceiver(), tp);
      gui_right->AddComponent(new ControlOutline(), tp);
      gui_right->AddComponent(new CameraRotateAction(CameraAction::TurnRight), tp);
      gui_right->AddComponent(new ControlText(">", "fonts/OrbitronLight.ttf", Vector4f(1, 1, 1, 1)), tp);
      gui_right->AddComponent(new StateListener(GameStates::Build, true), tp);
      tp.Spawn(gui_right);

      GameObject* gui_left = new GameObject();
      gui_left->AddComponent(new ControlArea(UDim(Vector2f(0.0f, 1.0f), Vector2f(10.0f, -10.0f), Edge::BottomLeft),
                                             UDim(Vector2f(0.20f, 0.1f), Vector2f( 0.0f,  10.0f))), tp);
      gui_left->AddComponent(new ControlEventReceiver(), tp);
      gui_left->AddComponent(new ControlOutline(), tp);
      gui_left->AddComponent(new CameraRotateAction(CameraAction::TurnLeft), tp);
      gui_left->AddComponent(new ControlText("<", "fonts/OrbitronLight.ttf", Vector4f(1, 1, 1, 1)), tp);
      gui_left->AddComponent(new StateListener(GameStates::Build, true), tp);
      tp.Spawn(gui_left);

      GameObject* gui_zoom = new GameObject();
      gui_zoom->AddComponent(new ControlArea(UDim(Vector2f(0.2f, 1.0f), Vector2f(20.0f, -10.0f), Edge::BottomLeft),
                                             UDim(Vector2f(0.20f, 0.1f), Vector2f( 0.0f,  10.0f))), tp);
      gui_zoom->AddComponent(new ControlEventReceiver(), tp);
      gui_zoom->AddComponent(new ControlOutline(), tp);
      gui_zoom->AddComponent(new CameraRotateAction(CameraAction::ZoomToggle), tp);
      gui_zoom->AddComponent(new ControlText("+", "fonts/OrbitronLight.ttf", Vector4f(1, 1, 1, 1)), tp);
      gui_zoom->AddComponent(new StateListener(GameStates::Build, true), tp);
      tp.Spawn(gui_zoom);

      
      GameObject* gui_mode = new GameObject();
      gui_mode->AddComponent(new ControlArea(UDim(Vector2f(0.8f, 1.0f), Vector2f(-20.0f, -10.0f), Edge::BottomRight),
                                             UDim(Vector2f(0.20f, 0.1f), Vector2f( 0.0f,  10.0f))), tp);
      gui_mode->AddComponent(new ControlEventReceiver(), tp);
      gui_mode->AddComponent(new ControlOutline(), tp);
      gui_mode->AddComponent(new CameraRotateAction(CameraAction::PanRotateToggle), tp);
      gui_mode->AddComponent(new ControlText("R", "fonts/OrbitronLight.ttf", Vector4f(1, 1, 1, 1)), tp);
      gui_mode->AddComponent(new StateListener(GameStates::Build, true), tp);
      tp.Spawn(gui_mode);
   }

   GameObject* path_vis = new GameObject();
   path_vis->AddComponent(new PathVisualiser(mBlocks), tp);
   path_vis->AddComponent(new StateListener(GameStates::Build, true), tp);
   tp.Spawn(path_vis);

   //Spawn a walker (move to defend GS when done)
   GameObject* walker = new GameObject();
   Position* pos = new Position();
   pos->SetPosition(Vector3f(0, 0, 0));
   walker->AddComponent(pos, tp);
   walker->AddComponent(new Navigator(), tp);
   walker->AddComponent(new CubeDrawer(), tp);
   walker->AddComponent(new WorldHandle(mBlocks), tp);
   walker->AddComponent(new StateListener(GameStates::Build, GameStates::Defend, true), tp); //Ditto make defend only
   tp.Spawn(walker);

}
