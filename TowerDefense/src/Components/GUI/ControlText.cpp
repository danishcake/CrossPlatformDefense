#include "ControlText.h"
#include "ControlArea.h"
#include "../../GameObject.h"
#include "../../Camera.h"
#include "../../Log.h"
#include "../../CheckMacros.h"

struct ControlTextVertex
{
   Vector4f pos;
   Vector2f tx;
};

ControlText::ControlText(std::string text, std::string font, Vector4f color)
 : mEffect(Effect::GetEffect("glsl/GUITextTransform.glsl", "glsl/GUITextFragment.glsl")),
   mText(text),
   mFont(font),
   mColor(color),
   mControlArea(0)
{
}

void ControlText::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mControlArea = owner->GetComponent<ControlArea>();

   mTexture = Texture::GetTexture(mText, mFont, mControlArea->GetSize());
}

void ControlText::InitialiseGraphics(TickParameters& tp)
{
   mEffect->InitialiseGraphics(tp);
   mTexture->InitialiseGraphics(tp);

   mPositionAttributeHandle = mEffect->GetAttributeLocation("InputPosition");
   mTxCoordAttributeHandle = mEffect->GetAttributeLocation("InputTextureCoord");
   mTransformUniformHandle = mEffect->GetUniformLocation("Transform");
   mColorUniformHandle = mEffect->GetUniformLocation("TextColor");

   //Initialise VBO for fill
   GL_CHECK(glGenBuffers(1, &mFillVBO));

   mFillVBOsize = 6;

   ControlTextVertex* verts = new ControlTextVertex[mFillVBOsize];

   Vector2f resolution = tp.camera->GetResolution();
   UDim& position = mControlArea->GetPosition();
   UDim& size = mControlArea->GetSize();

   Vector2f control_size = size.ToScreen(resolution);

   const TextDefinition text_def = mTexture->GetText();

   Vector2f tl = (control_size - text_def.mMeasurement) / 2;
   Vector2f br = text_def.mMeasurement + tl;

   verts[0].pos = Vector4f(position.ToScreen(resolution, control_size)) +
                  Vector4f(tl.x, tl.y, 0, 1);
   verts[0].tx  = Vector2f(0, 0.0f);

   verts[1].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(br.x, tl.y, 0, 1);
   verts[1].tx  = Vector2f(text_def.mTX.x, 0.0f);

   verts[2].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(br.x, br.y, 0, 1);
   verts[2].tx  = Vector2f(text_def.mTX.x, text_def.mTX.y);

   verts[3].pos = Vector4f(position.ToScreen(resolution, control_size)) +
                  Vector4f(br.x, br.y, 0, 1);
   verts[3].tx  = Vector2f(text_def.mTX.x, text_def.mTX.y);

   verts[4].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(tl.x, br.y, 0, 1);
   verts[4].tx  = Vector2f(0, text_def.mTX.y);

   verts[5].pos = Vector4f(position.ToScreen(resolution, control_size)) + 
                  Vector4f(tl.x, tl.y, 0, 1);
   verts[5].tx  = Vector2f(0, 0);

   GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mFillVBO));
   GL_CHECK(glBufferData(GL_ARRAY_BUFFER, mFillVBOsize * sizeof(ControlTextVertex), verts, GL_STATIC_DRAW));

   delete[] verts;
}

void ControlText::Tick(TickParameters& tp)
{
   tp.draw_list.AddToLayer(DrawLayer::GUI, this);
}

void ControlText::Draw(TickParameters& tp)
{
   /* Set shader,
      bind VBO with vertices in
      Set attribute point of position input to currently bound buffer (the vbo, with zero offset and stride)
      Connect the position input to the VBO (always the VBO bound to GL_ARRAY_BUFFER)
      */
   mEffect->UseEffect();
   mTexture->UseTexture(0);
   //Set camera and model position
   Matrix4f mvpm = Matrix4f::createOrtho(0, tp.camera->GetResolution().x, tp.camera->GetResolution().y, 0, 0, 10240) * mTransform;

   GL_CHECK(glUniformMatrix4fv(mTransformUniformHandle, 1, false, (float*)&mvpm));
   GL_CHECK(glUniform4fv(mColorUniformHandle, 1, (float*)&mColor));

   GL_CHECK(glEnableVertexAttribArray(mPositionAttributeHandle));
   GL_CHECK(glEnableVertexAttribArray(mTxCoordAttributeHandle));

   //Draw fill
   GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mFillVBO));
   GL_CHECK(glVertexAttribPointer(mPositionAttributeHandle, 4, GL_FLOAT, GL_FALSE, sizeof(ControlTextVertex), 0));
   GL_CHECK(glVertexAttribPointer(mTxCoordAttributeHandle, 2, GL_FLOAT, GL_FALSE, sizeof(ControlTextVertex), (void*)(4 * sizeof(float))));

   GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, mFillVBOsize));
}

void ControlText::TeardownGraphics(TickParameters& tp)
{
   GL_CHECK(glDeleteBuffers(1, &mFillVBO));
   tp.draw_list.RemoveFromLayer(DrawLayer::GUI, this);
}

void ControlText::SetTransform(Matrix4f transform)
{
   mTransform = transform;
}

void ControlText::SetAlpha(float a)
{
   mColor.a = a;
}