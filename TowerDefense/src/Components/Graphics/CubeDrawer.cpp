#include "CubeDrawer.h"
#include "../../Log.h"
#include "../../Camera.h"
#include "../Position.h"
#include "../../GameObject.h"

struct WorldVertex
{
   Vector4f pos;
   Vector2f texc;
};

CubeDrawer::CubeDrawer()
 : mTexture(Texture::GetTexture("textures/CubeTexture.png")),
   mEffect(Effect::GetEffect("glsl/CubeVertex.glsl", "glsl/CubeFragment.glsl")),
   mPosition(0)
{
}

void CubeDrawer::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mPosition = owner->GetComponent<Position>();
}

void CubeDrawer::buildCube()
{
   const Vector4f v0(0, 0, 0, 1);
   const Vector4f v1(0, 1, 0, 1);
   const Vector4f v2(0, 1, 1, 1);
   const Vector4f v3(0, 0, 1, 1);
   const Vector4f v4(1, 0, 0, 1);
   const Vector4f v5(1, 1, 0, 1);
   const Vector4f v6(1, 1, 1, 1);
   const Vector4f v7(1, 0, 1, 1);

   const float txe = 0.001f;
   const float tx0 = 0 + txe;
   const float tx1 = 0.5f - txe;
   const float ty0 = 0 + txe;
   const float ty1 = 0.5f - txe;
   const Vector2f t0(tx0, ty0);
   const Vector2f t1(tx1, ty0);
   const Vector2f t2(tx0, ty1);
   const Vector2f t3(tx1, ty1);

   const Vector4f offsets[] = {
         v3, v0, v1,   v3, v1, v2, //Near xaxis
         v4, v7, v6,   v4, v6, v5, //Far xaxis
         v0, v4, v5,   v0, v5, v1, //Near zaxis
         v7, v3, v2,   v7, v2, v6, //Far zaxis
         v4, v0, v3,   v4, v3, v7, //Near y axis (bottom)
         v1, v5, v6,   v1, v6, v2, //Far y axis (top)
   };

   const Vector2f texc_offsets[] = {
         t0, t1, t3,   t0, t3, t2,
         t0, t1, t3,   t0, t3, t2,
         t0, t1, t3,   t0, t3, t2,
         t0, t1, t3,   t0, t3, t2,
         t0, t1, t2,   t2, t3, t0,
         t0, t2, t1,   t2, t0, t3
   };

   WorldVertex* verts = new WorldVertex[36];


   //Create cube faces
   /*
    * First 24 vertices describe sides of cube, then the bottom 6, then the top 6
    * Cube texture is packed into single 2:1 texture
    */
   const Vector4f v_origin(0, 0, 0, 0);
   const Vector2f t_origin_side    (0.0f, 0.0f);
   const Vector2f t_origin_side_top(0.5f, 0.0f);

   //Sides and bottom
   for(int i = 0; i < 30; i++)
   {
      verts[i].pos  = v_origin + offsets[i];
      verts[i].texc = t_origin_side + texc_offsets[i];
   }

   //top
   for(int i = 30; i < 36; i++)
   {
      verts[i].pos  = v_origin + offsets[i];
      verts[i].texc = t_origin_side_top + texc_offsets[i];
   }
   mVBOsize = 36;

   glBindBuffer(GL_ARRAY_BUFFER, mVBO);
   glBufferData(GL_ARRAY_BUFFER, mVBOsize * sizeof(WorldVertex), verts, GL_STATIC_DRAW);
   delete[] verts;
}

void CubeDrawer::InitialiseGraphics(TickParameters& tp)
{
   mEffect->InitialiseGraphics(tp);
   mTexture->InitialiseGraphics(tp);

   mPositionAttributeHandle = mEffect->GetAttributeLocation("InputPosition");
   mTexcAttributeHandle = mEffect->GetAttributeLocation("InputTextureCoord");
   mTransformUniformHandle = mEffect->GetUniformLocation("Transform");

   //Initialise VBO
   glGenBuffers(1, &mVBO);
   buildCube();
}

void CubeDrawer::Tick(TickParameters& tp)
{
   tp.draw_list.AddToLayer(DrawLayer::Game, this);
}

void CubeDrawer::Draw(TickParameters& tp)
{
   /* Set shader,
      bind VBO with vertices in
      Set attribute point of position input to currently bound buffer (the vbo, with zero offset and stride)
      Connect the position input to the VBO (always the VBO bound to GL_ARRAY_BUFFER)
      */
   mEffect->UseEffect();
   mTexture->UseTexture(0);
   //Set camera and model position
   Vector3f pos = mPosition->GetPosition();
   Matrix4f mvpm = tp.camera->GetViewProjection() * Matrix4f::createTranslation(pos.x, pos.y, pos.z);

   glUniformMatrix4fv(mTransformUniformHandle, 1, false, (float*)&mvpm);

   glEnableVertexAttribArray(mPositionAttributeHandle);
   glEnableVertexAttribArray(mTexcAttributeHandle);

   glBindBuffer(GL_ARRAY_BUFFER, mVBO);
   glVertexAttribPointer(mPositionAttributeHandle, 4, GL_FLOAT, GL_FALSE, sizeof(WorldVertex), 0);
   glVertexAttribPointer(mTexcAttributeHandle, 2, GL_FLOAT, GL_FALSE, sizeof(WorldVertex), (void*)(4 * sizeof(float)));

   glDrawArrays(GL_TRIANGLES, 0, mVBOsize);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CubeDrawer::TeardownGraphics(TickParameters& tp)
{
   glDeleteBuffers(1, &mVBO);
   tp.draw_list.RemoveFromLayer(DrawLayer::Game, this);
}
