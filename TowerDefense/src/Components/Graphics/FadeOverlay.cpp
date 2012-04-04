#include "FadeOverlay.h"
#include "../../GameObject.h"

FadeOverlay::FadeOverlay(FadeType::Enum fade_type, Vector3f fade_color, float fade_time)
   :  mEffect(Effect::GetEffect("glsl/FullscreenVertex.glsl", "glsl/Fade.glsl")),
      mFadeColor(fade_color),
      mFadeTime(0),
      mFadeTimer(fade_time),
      mFadeScalar(0),
      mFadeType(fade_type)
{
}

void FadeOverlay::InitialiseGraphics(TickParameters& tp)
{
   mEffect->InitialiseGraphics(tp);
   mPositionAttributeHandle = mEffect->GetAttributeLocation("Position");
   mFadeScalarUniformHandle = mEffect->GetUniformLocation("FadeScalar");
   mFadeColorUniformHandle = mEffect->GetUniformLocation("FadeColor");

   //Initialise VBO
   Vector2f verts[6];
   verts[0] = Vector2f(0, 0);
   verts[1] = Vector2f( 1, 0);
   verts[2] = Vector2f( 0,  1);

   verts[3] = Vector2f( 1,  0);
   verts[4] = Vector2f( 1,  1);
   verts[5] = Vector2f( 0,  1);

   glGenBuffers(1, &mVBO);
   glBindBuffer(GL_ARRAY_BUFFER, mVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
   mVBOsize = sizeof(verts) / sizeof(Vector2f);
}

void FadeOverlay::Tick(TickParameters& tp)
{
   mFadeTime += tp.timespan;
   mFadeScalar = mFadeTime / mFadeTimer;
   
   if(mFadeScalar > 1.0f)
   {
      if(mFadeType == FadeType::FadeOutThenIn)
      {
         mFadeType = FadeType::FadeIn;
         mFadeTime = 0;
         mFadeScalar = 0.0f;
      } else
      {
         mOwner->Kill();
      }
   }

   if(!GameObject::IsDead(mOwner))
   {
      if(mFadeType == FadeType::FadeIn)
         mFadeScalar = 1.0f - mFadeScalar;
      tp.draw_list.AddToLayer(DrawLayer::Fade, this);
   }
}

void FadeOverlay::Draw(TickParameters& tp)
{
   /* Set shader,
      bind VBO with vertices in
      Set attribute point of position input to currently bound buffer (the vbo, with zero offset and stride)
      Connect the position input to the VBO (always the VBO bound to GL_ARRAY_BUFFER)
      */
   mEffect->UseEffect();

   glUniform3f(mFadeColorUniformHandle, mFadeColor.r, mFadeColor.g, mFadeColor.b);
   glUniform1f(mFadeScalarUniformHandle, mFadeScalar);

   glBindBuffer(GL_ARRAY_BUFFER, mVBO);
   glVertexAttribPointer(mPositionAttributeHandle, 2, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(mPositionAttributeHandle);
   glDrawArrays(GL_TRIANGLES, 0, mVBOsize);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FadeOverlay::TeardownGraphics(TickParameters& tp)
{
   glDeleteBuffers(1, &mVBO);
   tp.draw_list.RemoveFromLayer(DrawLayer::Game, this);
}
