#include "ControlOutline.h"
#include "ControlArea.h"
#include "ControlEventReceiver.h"
#include "../../GameObject.h"
#include "../../Camera.h"
#include "../../Log.h"
#include "../../CheckMacros.h"

struct ControlVertex
{
   Vector4f pos;
   Vector4f col;
};

ControlOutline::ControlOutline()
 : mEffect(Effect::GetEffect("glsl/GUITransform.glsl", "glsl/GUIFragment.glsl")),
   mControlArea(0),
   mEventRcvr(0),
   mAlpha(1.0f)
{
}

void ControlOutline::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mControlArea = owner->GetComponent<ControlArea>();
   mEventRcvr = owner->GetComponent<ControlEventReceiver>();
}

void ControlOutline::InitialiseGraphics(TickParameters& tp)
{
   mEffect->InitialiseGraphics(tp);

   mPositionAttributeHandle = mEffect->GetAttributeLocation("InputPosition");
   mColorAttributeHandle = mEffect->GetAttributeLocation("InputColor");
   mTransformUniformHandle = mEffect->GetUniformLocation("Transform");
   mAlphaUniformHandle= mEffect->GetUniformLocation("VertexAlpha");

   //Initialise VBO for fill
   GL_CHECK(glGenBuffers(1, &mFillVBO));
   GL_CHECK(glGenBuffers(1, &mOutlineVBO));

   mFillVBOsize = 6;

   ControlVertex* verts = new ControlVertex[mFillVBOsize];

   Vector2f resolution = tp.camera->GetResolution();
   UDim& position = mControlArea->GetPosition();
   UDim& size = mControlArea->GetSize();

   Vector2f control_size = size.ToScreen(resolution);

   verts[0].pos = Vector4f(position.ToScreen(resolution, control_size)) +
                  Vector4f(0, 0, 0, 1);
   verts[0].col = Vector4f(0, 0, 1, 0.5f);

   verts[1].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(control_size.x, 0, 0, 1);
   verts[1].col = Vector4f(0, 0, 1, 0.5f);

   verts[2].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(control_size.x, control_size.y, 0, 1);
   verts[2].col = Vector4f(0, 0, 1, 0.5f);

   verts[3].pos = Vector4f(position.ToScreen(resolution, control_size)) +
                  Vector4f(control_size.x, control_size.y, 0, 1);
   verts[3].col = Vector4f(0, 0, 1, 0.5f);

   verts[4].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(0, control_size.y, 0, 1);
   verts[4].col = Vector4f(0, 0, 1, 0.5f);

   verts[5].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(0, 0, 0, 1);
   verts[5].col = Vector4f(0, 0, 1, 0.5f);

   GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mFillVBO));
   GL_CHECK(glBufferData(GL_ARRAY_BUFFER, mFillVBOsize * sizeof(ControlVertex), verts, GL_STATIC_DRAW));

   delete[] verts;

   //Initialise VBO for outline

   //Two outlines for 16 verts
   mOutlineVBOsize = 16;
   verts = new ControlVertex[mOutlineVBOsize];
   verts[0].pos = Vector4f(position.ToScreen(resolution, control_size)) +
                  Vector4f(0, 0, 0, 1);
   verts[0].col = Vector4f(1, 1, 1, 1.0f);

   verts[1].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(control_size.x, 0, 0, 1);
   verts[1].col = Vector4f(1, 1, 1, 1.0f);


   verts[2].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(control_size.x, 0, 0, 1);
   verts[2].col = Vector4f(1, 1, 1, 1.0f);

   verts[3].pos = Vector4f(position.ToScreen(resolution, control_size)) +
                  Vector4f(control_size.x, control_size.y, 0, 1);
   verts[3].col = Vector4f(1, 1, 1, 1.0f);


   verts[4].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(control_size.x, control_size.y, 0, 1);
   verts[4].col = Vector4f(1, 1, 1, 1.0f);

   verts[5].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(0, control_size.y, 0, 1);
   verts[5].col = Vector4f(1, 1, 1, 1.0f);

   verts[6].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(0, control_size.y, 0, 1);
   verts[6].col = Vector4f(1, 1, 1, 1.0f);

   verts[7].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(0, 0, 0, 1);
   verts[7].col = Vector4f(1, 1, 1, 1.0f);



   verts[8].pos = Vector4f(position.ToScreen(resolution, control_size)) +
                  Vector4f(2, 2, 0, 1);
   verts[8].col = Vector4f(1, 1, 1, 1.0f);

   verts[9].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(control_size.x - 2, 2, 0, 1);
   verts[9].col = Vector4f(1, 1, 1, 1.0f);


   verts[10].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                   Vector4f(control_size.x - 2, 2, 0, 1);
   verts[10].col = Vector4f(1, 1, 1, 1.0f);

   verts[11].pos = Vector4f(position.ToScreen(resolution, control_size)) +
                   Vector4f(control_size.x - 2, control_size.y - 2, 0, 1);
   verts[11].col = Vector4f(1, 1, 1, 1.0f);


   verts[12].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                   Vector4f(control_size.x - 2, control_size.y - 2, 0, 1);
   verts[12].col = Vector4f(1, 1, 1, 1.0f);

   verts[13].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                   Vector4f(2, control_size.y - 2, 0, 1);
   verts[13].col = Vector4f(1, 1, 1, 1.0f);

   verts[14].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                   Vector4f(2, control_size.y - 2, 0, 1);
   verts[14].col = Vector4f(1, 1, 1, 1.0f);

   verts[15].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                   Vector4f(2, 2, 0, 1);
   verts[15].col = Vector4f(1, 1, 1, 1.0f);

   GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mOutlineVBO));
   GL_CHECK(glBufferData(GL_ARRAY_BUFFER, mOutlineVBOsize * sizeof(ControlVertex), verts, GL_STATIC_DRAW));

   delete[] verts;
}

void ControlOutline::Tick(TickParameters& tp)
{
   if(!mEventRcvr || !mEventRcvr->GetClicked())
      tp.draw_list.AddToLayer(DrawLayer::GUI, this);
}

void ControlOutline::Draw(TickParameters& tp)
{
   /* Set shader,
      bind VBO with vertices in
      Set attribute point of position input to currently bound buffer (the vbo, with zero offset and stride)
      Connect the position input to the VBO (always the VBO bound to GL_ARRAY_BUFFER)
      */
   mEffect->UseEffect();
   //Set camera and model position
   Matrix4f mvpm = Matrix4f::createOrtho(0, tp.camera->GetResolution().x, tp.camera->GetResolution().y, 0, 0, 10240) * mTransform;

   GL_CHECK(glUniformMatrix4fv(mTransformUniformHandle, 1, false, (float*)&mvpm));
   GL_CHECK(glUniform1f(mAlphaUniformHandle, mAlpha));

   GL_CHECK(glEnableVertexAttribArray(mPositionAttributeHandle));
   GL_CHECK(glEnableVertexAttribArray(mColorAttributeHandle));

   //Draw fill
   GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mFillVBO));
   GL_CHECK(glVertexAttribPointer(mPositionAttributeHandle, 4, GL_FLOAT, GL_FALSE, sizeof(ControlVertex), 0));
   GL_CHECK(glVertexAttribPointer(mColorAttributeHandle, 4, GL_FLOAT, GL_FALSE, sizeof(ControlVertex), (void*)(4 * sizeof(float))));

   GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, mFillVBOsize));

   //Draw outline
   GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mOutlineVBO));
   GL_CHECK(glVertexAttribPointer(mPositionAttributeHandle, 4, GL_FLOAT, GL_FALSE, sizeof(ControlVertex), 0));
   GL_CHECK(glVertexAttribPointer(mColorAttributeHandle, 4, GL_FLOAT, GL_FALSE, sizeof(ControlVertex), (void*)(4 * sizeof(float))));

   GL_CHECK(glDrawArrays(GL_LINES, 0, mOutlineVBOsize));

   GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void ControlOutline::TeardownGraphics(TickParameters& tp)
{
   GL_CHECK(glDeleteBuffers(1, &mFillVBO));
   GL_CHECK(glDeleteBuffers(1, &mOutlineVBO));
   tp.draw_list.RemoveFromLayer(DrawLayer::GUI, this);
}

void ControlOutline::SetTransform(Matrix4f transform)
{
   mTransform = transform;
}

void ControlOutline::SetAlpha(float a)
{
   mAlpha = a;
}
