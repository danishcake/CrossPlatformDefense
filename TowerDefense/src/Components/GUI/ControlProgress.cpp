#include "ControlProgress.h"
#include "ControlArea.h"
#include "../../GameObject.h"
#include "../../Camera.h"
#include "../../Log.h"
#include "../../CheckMacros.h"

struct ProgressVertex
{
   Vector4f pos;
   float col;
};

ControlProgress::ControlProgress()
 : mEffect(Effect::GetEffect("glsl/GUIProgressTransform.glsl", "glsl/GUIProgressFragment.glsl")),
   mControlArea(0),
   mAlpha(1.0f),
   mProgress(0)
{
}

void ControlProgress::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mControlArea = owner->GetComponent<ControlArea>();
}

void ControlProgress::InitialiseGraphics(TickParameters& tp)
{
   mEffect->InitialiseGraphics(tp);

   
   mTransformUniformHandle = mEffect->GetUniformLocation("Transform");
   mVertexColorLUTUniformHandle = mEffect->GetUniformLocation("VertexColorLUT");
   mAlphaUniformHandle = mEffect->GetUniformLocation("VertexAlpha");
   mWidthUniformHandle = mEffect->GetUniformLocation("ProgressWidth");

   mPositionAttributeHandle = mEffect->GetAttributeLocation("InputPosition");
   mColorAttributeHandle = mEffect->GetAttributeLocation("InputVertexColorIndex");


   //Initialise VBO for fill
   GL_CHECK(glGenBuffers(1, &mFillVBO));

   mFillVBOsize = 6;

   ProgressVertex verts[6];

   Vector2f resolution = tp.camera->GetResolution();
   UDim& position = mControlArea->GetPosition();
   UDim& size = mControlArea->GetSize();

   Vector2f control_size = size.ToScreen(resolution);
   const float inset = 0.0f;
   mFullWidth = (control_size.x - 2 * inset);

   verts[0].pos = Vector4f(position.ToScreen(resolution, control_size)) +
                  Vector4f(inset, inset, 0, 1);
   verts[0].col = 0;

   verts[1].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(inset, inset, 1, 1);
   verts[1].col = 1;

   verts[2].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(inset, control_size.y - inset, 1, 1);
   verts[2].col = 1;

   verts[3].pos = Vector4f(position.ToScreen(resolution, control_size)) +
                  Vector4f(inset, control_size.y - inset, 1, 1);
   verts[3].col = 1;

   verts[4].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(inset, control_size.y - inset, 0, 1);
   verts[4].col = 0;

   verts[5].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(inset, inset, 0, 1);
   verts[5].col = 0;

   GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mFillVBO));
   GL_CHECK(glBufferData(GL_ARRAY_BUFFER, mFillVBOsize * sizeof(ProgressVertex), verts, GL_STATIC_DRAW));
}

void ControlProgress::Tick(TickParameters& tp)
{
   tp.draw_list.AddToLayer(DrawLayer::GUI, this);
}

void ControlProgress::Draw(TickParameters& tp)
{
   const Vector4f colors[2] = { Vector4f(0.43921568627450980392f, 0.57254901960784313725f, 0.74509803921568627451f, 1.0f),
                                Vector4f(0.6f, 0.8509803921568627451f, 0.91764705882352941176f, 1.0f) };
                  

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

   GL_CHECK(glUniform4fv(mVertexColorLUTUniformHandle, 2, (float*)&colors));
   GL_CHECK(glUniform1f(mAlphaUniformHandle, mAlpha));
   GL_CHECK(glUniform1f(mWidthUniformHandle, mProgress * mFullWidth));

   GL_CHECK(glEnableVertexAttribArray(mPositionAttributeHandle));
   GL_CHECK(glEnableVertexAttribArray(mColorAttributeHandle));

   //Draw fill
   GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mFillVBO));
   GL_CHECK(glVertexAttribPointer(mPositionAttributeHandle, 4, GL_FLOAT, GL_FALSE, sizeof(ProgressVertex), 0));
   GL_CHECK(glVertexAttribPointer(mColorAttributeHandle, 1, GL_FLOAT, GL_FALSE, sizeof(ProgressVertex), (void*)(4 * sizeof(float))));

   GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, mFillVBOsize));
   
   GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void ControlProgress::TeardownGraphics(TickParameters& tp)
{
   GL_CHECK(glDeleteBuffers(1, &mFillVBO));
   tp.draw_list.RemoveFromLayer(DrawLayer::GUI, this);
}

void ControlProgress::SetTransform(Matrix4f transform)
{
   mTransform = transform;
}

void ControlProgress::SetAlpha(float a)
{
   mAlpha = a;
}

void ControlProgress::SetProgress(float progress)
{
   mProgress = progress;
   mProgress = std::max(0.0f, mProgress);
   mProgress = std::min(1.0f, mProgress);
}