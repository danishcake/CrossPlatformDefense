#include "TextDrawer.h"
#include "../../CheckMacros.h"
#include "../../Log.h"
#include "../../Camera.h"

struct TextVertex
{
   Vector4f pos;
   Vector4f col;
   Vector2f tx;
};

TextDrawer::TextDrawer(std::string font, std::string text)
   : mText(text), mFont(Font::GetFont(font)),
     mEffect(Effect::GetEffect("glsl/TextTransform.glsl", "glsl/TextFragment.glsl"))
{
}

TextDrawer::TextDrawer(std::string font, std::string text, Matrix4f transform)
   : mText(text), mFont(Font::GetFont(font)),
     mEffect(Effect::GetEffect("glsl/TextTransform.glsl", "glsl/TextFragment.glsl")),
     mTransform(transform)
{
}

void TextDrawer::InitialiseGraphics(TickParameters& tp)
{
   mFont->InitialiseGraphics(tp);
   mEffect->InitialiseGraphics(tp);

   mPositionAttributeHandle = mEffect->GetAttributeLocation("InputPosition");
   mColorAttributeHandle = mEffect->GetAttributeLocation("InputColor");
   mTextureCoordinateAttributeHandle = mEffect->GetAttributeLocation("InputTextureCoords");
   mTransformUniformHandle = mEffect->GetUniformLocation("Transform");


   //Now build a VBO with verts laying out the font
   GL_CHECK(glGenBuffers(1, &mVBO));

   mVBOsize = mText.length() * 6;

   TextVertex* verts = new TextVertex[mVBOsize];


   float left = 0;
   float min_y = 10000;
   float max_y = -10000;
   float min_x = 10000;
   float max_x = -10000;
   char c;
   char c_prev = 0;
   Vector4f color(1, 1, 1, 0);
   for(unsigned int i = 0; i < mText.length(); i++)
   {
      c = mText.c_str()[i];

      float tex_left  = (c % 16) * (1.0f / 16.0f);
      float tex_right = tex_left + 1.0f / 16.0f;

      float tex_top = (c / 16) * (1.0f / 16.0f);
      float tex_bottom = tex_top + 1.0f / 16.0f;

      float right = left + 16.0f;
      float top = static_cast<float>(-mFont->GetYOffset(c));
      float bottom = top + 16.0f;

      if(top < min_y)
         min_y = top;
      if(bottom > max_y)
         max_y = bottom;
      if(left < min_x)
         min_x = left;
      if(right > max_x)
         max_x = right;


      verts[i*6 + 0].pos = Vector4f(left, top, 0, 1);
      verts[i*6 + 0].col = color;
      verts[i*6 + 0].tx = Vector2f(tex_left, tex_top);

      verts[i*6 + 1].pos = Vector4f(right, top, 0, 1);
      verts[i*6 + 1].col = color;
      verts[i*6 + 1].tx = Vector2f(tex_right, tex_top);

      verts[i*6 + 2].pos = Vector4f(left, bottom, 0, 1);
      verts[i*6 + 2].col = color;
      verts[i*6 + 2].tx = Vector2f(tex_left, tex_bottom);


      verts[i*6 + 3].pos = Vector4f(right, top, 0, 1);
      verts[i*6 + 3].col = color;
      verts[i*6 + 3].tx = Vector2f(tex_right, tex_top);

      verts[i*6 + 4].pos = Vector4f(right, bottom, 0, 1);
      verts[i*6 + 4].col = color;
      verts[i*6 + 4].tx = Vector2f(tex_right, tex_bottom);

      verts[i*6 + 5].pos = Vector4f(left, bottom, 0, 1);
      verts[i*6 + 5].col = color;
      verts[i*6 + 5].tx = Vector2f(tex_left, tex_bottom);


      left += std::ceilf(mFont->GetAdvance(c));
      if(c_prev)
         left += ((float)mFont->GetKern(c_prev, c));
      c_prev = c;
   }
   //Now adjust so top left aligns with 0, 0

   for(unsigned int i = 0; i < mText.length(); i++)
   {
      verts[i*6 + 0].pos.y -= min_y;
      verts[i*6 + 1].pos.y -= min_y;
      verts[i*6 + 2].pos.y -= min_y;
      verts[i*6 + 3].pos.y -= min_y;
      verts[i*6 + 4].pos.y -= min_y;
      verts[i*6 + 5].pos.y -= min_y;
   }


   mWidth = max_x - min_x;
   mHeight = max_y - min_y;

   glBindBuffer(GL_ARRAY_BUFFER, mVBO);
   glBufferData(GL_ARRAY_BUFFER, mVBOsize * sizeof(TextVertex), verts, GL_STATIC_DRAW);
   delete[] verts;
}

void TextDrawer::Tick(TickParameters& tp)
{
   tp.draw_list.AddToLayer(DrawLayer::GUI, this);
}

void TextDrawer::Draw(TickParameters& tp)
{
   mFont->GetTexture()->UseTexture(0);
   mEffect->UseEffect();

   Matrix4f mvpm = Matrix4f::createOrtho(0, tp.camera->GetResolution().x, tp.camera->GetResolution().y, 0, 0, 10240) * mTransform;
   GL_CHECK(glUniformMatrix4fv(mTransformUniformHandle, 1, false, (float*)&mvpm));

   GL_CHECK(glEnableVertexAttribArray(mPositionAttributeHandle));
   GL_CHECK(glEnableVertexAttribArray(mColorAttributeHandle));
   GL_CHECK(glEnableVertexAttribArray(mTextureCoordinateAttributeHandle));

   //Draw characters
   GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mVBO));
   GL_CHECK(glVertexAttribPointer(mPositionAttributeHandle, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), 0));
   GL_CHECK(glVertexAttribPointer(mColorAttributeHandle, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(4 * sizeof(float))));
   GL_CHECK(glVertexAttribPointer(mTextureCoordinateAttributeHandle, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(8 * sizeof(float))));

   GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, mVBOsize));
}

void TextDrawer::TeardownGraphics(TickParameters& tp)
{
   glDeleteBuffers(1, &mVBO);
}
