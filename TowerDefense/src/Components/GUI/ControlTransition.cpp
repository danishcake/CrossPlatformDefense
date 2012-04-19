#include "ControlTransition.h"
#include "ControlText.h"
#include "ControlArea.h"
#include "ControlOutline.h"
#include "ControlProgress.h"
#include "../../Camera.h"
#include "../../GameObject.h"
#include "../../Log.h"

ControlTransition::ControlTransition(ControlTransitionState::Enum state, float period, int control_transition_group) :
   mTransitionTimer(0),
   mTransitionPeriod(period),
   mPendingState(state),
   mTransitionID(control_transition_group)
{
}

void ControlTransition::Initialise(TickParameters& tp, GameObject* owner)
{
   mTransitionControlSubscriber.Initialise(&tp.msg);
   Component::Initialise(tp, owner);
   mControlArea = owner->GetComponent<ControlArea>();
   mControlOutline = owner->GetComponent<ControlOutline>();
   mControlText = owner->GetComponent<ControlText>();
   mControlProgress = owner->GetComponent<ControlProgress>();
}

void ControlTransition::Tick(TickParameters& tp)
{
   std::vector<GuiTransitionControlMessage> tcm = mTransitionControlSubscriber.GetMessages();
   for(std::vector<GuiTransitionControlMessage>::iterator it = tcm.begin(); it != tcm.end(); ++it)
   {
      if(it->TransitionID == mTransitionID)
      {
         if(it->TransitionType == GuiTransition::TransitionIn)
         {
            mTransitionTimer = 0;
            mPendingState = ControlTransitionState::TransIn;
         }
         if(it->TransitionType == GuiTransition::TransitionOut)
         {
            mTransitionTimer = 0;
            mPendingState = ControlTransitionState::TransOut;
         }
      }
   }


   mTransitionTimer += tp.timespan;

   float scalar_alpha = 1.0f;
   float scalar_size = 1.0f;
   switch(mPendingState)
   {
      case ControlTransitionState::TransIn:
         scalar_alpha = smootherstep(0.0f, 1.0f, mTransitionTimer / mTransitionPeriod);
         scalar_size = 0.5f + 0.5f * scalar_alpha;
         break;
      case ControlTransitionState::TransOut:
         scalar_alpha = 1.0f - smootherstep(0.0f, 1.0f, mTransitionTimer / mTransitionPeriod);
         scalar_size = 1.0f + 0.5f * smootherstep(0.0f, 1.0f, mTransitionTimer / mTransitionPeriod);
         scalar_size = clamp(scalar_size, 0.0f, 1.5f);
         break;
      case ControlTransitionState::Hidden:
         scalar_alpha = 0.0f;
         scalar_size = 0.0f;
         break;
      case ControlTransitionState::Visible:
      default:
         scalar_alpha = 1.0f;
         scalar_size = 1.0f;
         break;
   }

   //scalar_alpha = sqrtf(scalar_alpha);

   //Scale size and transparency of rendered parts of the control
   if(mControlArea)
   {
      Vector2f size = mControlArea->GetSize().ToScreen(tp.camera->GetResolution());
      Vector2f top_left = mControlArea->GetPosition().ToScreen(tp.camera->GetResolution(), size);
      Matrix4f scale = Matrix4f::createTranslation(size.x / 2 + top_left.x, size.y / 2 + top_left.y, 0) * 
                       Matrix4f::createScale(scalar_size) * 
                       Matrix4f::createTranslation(-size.x / 2 - top_left.x, -size.y / 2 - top_left.y, 0);

      if(mControlOutline)
      {
         //Scale outline
         mControlOutline->SetTransform(scale);
         mControlOutline->SetAlpha(scalar_alpha);
      }
      
      if(mControlText)
      {
         //Scale text
         mControlText->SetTransform(scale);
         mControlText->SetAlpha(scalar_alpha);
      }

      if(mControlProgress)
      {
         //Scale progress bar
         mControlProgress->SetTransform(scale);
         mControlProgress->SetAlpha(scalar_alpha);
      }
   }
}

