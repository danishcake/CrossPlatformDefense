#include "TexturedQuad.h"
#include "../../Log.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

struct V2Textured {
   Vector2f pos;
   Vector2f texc;
};

TexturedQuad::TexturedQuad() :
   mEffect(Effect::GetEffect("glsl/FullscreenTexturedVertex.glsl", "glsl/FlatTextured.glsl")),
   mTexture(Texture::GetTexture("textures/TerrainTexture.png"))
{
}

TexturedQuad::TexturedQuad(TexturePtr texture)
   : mEffect(Effect::GetEffect("glsl/FullscreenTexturedVertex.glsl", "glsl/FlatTextured.glsl")),
     mTexture(texture)
{

}

void TexturedQuad::InitialiseGraphics(TickParameters& tp)
{
   mEffect->InitialiseGraphics(tp);
   mTexture->InitialiseGraphics(tp);

   mVertAttributeHandle = mEffect->GetAttributeLocation("pos");
   mTexcAttributeHandle = mEffect->GetAttributeLocation("texc");

   //Initialise VBO
   V2Textured verts[6];
   verts[0].pos = Vector2f( 0,  0);
   verts[1].pos = Vector2f( 1,  0);
   verts[2].pos = Vector2f( 0,  1);

   verts[3].pos = Vector2f( 1,  0);
   verts[4].pos = Vector2f( 1,  1);
   verts[5].pos = Vector2f( 0,  1);


   verts[0].texc = Vector2f(0, 0);
   verts[1].texc = Vector2f(1, 0);
   verts[2].texc = Vector2f(0, 1);

   verts[3].texc = Vector2f(1, 0);
   verts[4].texc = Vector2f(1, 1);
   verts[5].texc = Vector2f(0, 1);

   glGenBuffers(1, &mVBO);
   glBindBuffer(GL_ARRAY_BUFFER, mVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
   mVBOsize = sizeof(verts) / sizeof(V2Textured);
}

void TexturedQuad::Tick(TickParameters& tp)
{
   tp.draw_list.AddToLayer(DrawLayer::Game, this);
}

void TexturedQuad::Draw(TickParameters& tp)
{
   /* Set shader,
      bind VBO with vertices in
      Set attribute point of position input to currently bound buffer (the vbo, with zero offset and stride)
      Connect the position input to the VBO (always the VBO bound to GL_ARRAY_BUFFER)
      */
   mEffect->UseEffect();
   mTexture->UseTexture(0);


   glBindBuffer(GL_ARRAY_BUFFER, mVBO);

   glVertexAttribPointer(mVertAttributeHandle, 2, GL_FLOAT, GL_FALSE, sizeof(V2Textured), BUFFER_OFFSET(0));
   glEnableVertexAttribArray(mVertAttributeHandle);
   glVertexAttribPointer(mTexcAttributeHandle, 2, GL_FLOAT, GL_FALSE, sizeof(V2Textured), BUFFER_OFFSET(sizeof(float)*2));
   glEnableVertexAttribArray(mTexcAttributeHandle);
   glDrawArrays(GL_TRIANGLES, 0, 6);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TexturedQuad::TeardownGraphics(TickParameters& tp)
{
   glDeleteBuffers(1, &mVBO);
   tp.draw_list.RemoveFromLayer(DrawLayer::Game, this);
}
