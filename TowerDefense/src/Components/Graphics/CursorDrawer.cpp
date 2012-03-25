#include "CursorDrawer.h"
#include "../../Log.h"
#include "../../Camera.h"
#include "../Position.h"
#include "../../GameObject.h"

CursorDrawer::CursorDrawer()
 : mEffect(Effect::GetEffect("glsl/CursorVertex.glsl", "glsl/CursorFragment.glsl")),
   mPosition(0)
{
}

void CursorDrawer::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mPosition = owner->GetComponent<Position>();
}

void CursorDrawer::InitialiseGraphics(TickParameters& tp)
{
   mEffect->InitialiseGraphics(tp);

   mPositionAttributeHandle = mEffect->GetAttributeLocation("InputPosition");
   mTransformUniformHandle = mEffect->GetUniformLocation("Transform");

   //Initialise VBO
   glGenBuffers(1, &mVBO);
   const Vector4f verts[] =
   {
      Vector4f(0, 0.1f, 0, 1),
      Vector4f(1, 0.1f, 0, 1),
      Vector4f(1, 0.1f, 1, 1),
      Vector4f(0, 0.1f, 0, 1),
      Vector4f(1, 0.1f, 1, 1),
      Vector4f(0, 0.1f, 1, 1)
   };

   mVBOsize = 6;

   glBindBuffer(GL_ARRAY_BUFFER, mVBO);
   glBufferData(GL_ARRAY_BUFFER, mVBOsize * sizeof(Vector4f), verts, GL_STATIC_DRAW);
}

void CursorDrawer::Tick(TickParameters& tp)
{
   tp.draw_list.AddToLayer(DrawLayer::GUI, this);
}

void CursorDrawer::Draw(TickParameters& tp)
{
   /* Set shader,
      bind VBO with vertices in
      Set attribute point of position input to currently bound buffer (the vbo, with zero offset and stride)
      Connect the position input to the VBO (always the VBO bound to GL_ARRAY_BUFFER)
      */
   mEffect->UseEffect();
   //Set camera and model position
   Vector3f pos = mPosition->GetPosition();
   Matrix4f mvpm = tp.camera->GetViewProjection() * Matrix4f::createTranslation(pos.x, pos.y, pos.z);

   glUniformMatrix4fv(mTransformUniformHandle, 1, false, (float*)&mvpm);

   glEnableVertexAttribArray(mPositionAttributeHandle);

   glBindBuffer(GL_ARRAY_BUFFER, mVBO);
   glVertexAttribPointer(mPositionAttributeHandle, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4f), 0);

   glDrawArrays(GL_TRIANGLES, 0, mVBOsize);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CursorDrawer::TeardownGraphics(TickParameters& tp)
{
   glDeleteBuffers(1, &mVBO);
   tp.draw_list.RemoveFromLayer(DrawLayer::Game, this);
}
