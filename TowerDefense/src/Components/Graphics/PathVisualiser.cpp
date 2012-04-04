#include "PathVisualiser.h"
#include "../../Log.h"
#include "../../PotentialMap.h" //includes #include "../../CommonDefs.h"
#include "../../WorldBlocks.h"
#include "../../Camera.h"



PathVisualiser::PathVisualiser(WorldBlocks* blocks)
 :
   mEffect(Effect::GetEffect("glsl/Vector4fTransform.glsl", "glsl/SolidRed.glsl")),
   mPotentialMap(blocks, 1, 1),
   mWorldBlocks(blocks)
{
}

void PathVisualiser::InitialiseGraphics(TickParameters& tp)
{
   mEffect->InitialiseGraphics(tp);

   mPositionAttributeHandle = mEffect->GetAttributeLocation("InputPosition");
   mTransformUniformHandle = mEffect->GetUniformLocation("Transform");

   //Initialise VBO
   glGenBuffers(1, &mVBO);
   
   BuildMap();

}

void PathVisualiser::BuildMap()
{

   mVBOsize = 0;
   Vector4f* verts = new Vector4f[WORLD_WIDTH * WORLD_BREADTH * 3];
   //Build VBO
   for(int x = 0; x < WORLD_WIDTH; x++)
   {
      for(int z = 0; z < WORLD_BREADTH; z++)
      {
         int dx = 0;
         int dz = 0;
         mPotentialMap.Guide(x, z, dx, dz);
         if(dx != 0 || dz != 0)
         {
            /* |--          
             * |  --
             * |    -->
             * |  --
             * |--
             */
            float ang = std::atan2f(static_cast<float>(dz), static_cast<float>(dx));
            float scale = 0.5f;
            verts[mVBOsize + 0] = Vector4f(std::cos(DEG2RADF(-120.0f) + ang), 0, std::sin(DEG2RADF(-120.0f) + ang), 0) * scale;
            verts[mVBOsize + 1] = Vector4f(std::cos(DEG2RADF(0.0f) + ang), 0, std::sin(DEG2RADF(0.0f) + ang), 0) * scale;
            verts[mVBOsize + 2] = Vector4f(std::cos(DEG2RADF(120.0f) + ang), 0, std::sin(DEG2RADF(120.0f) + ang), 0) * scale;

            float height = mWorldBlocks->getColumn(x, z).GetHeight() + 1.1f;
            verts[mVBOsize + 0] += Vector4f(0.5f, height, 0.5f, 0);
            verts[mVBOsize + 1] += Vector4f(0.5f, height, 0.5f, 0);
            verts[mVBOsize + 2] += Vector4f(0.5f, height, 0.5f, 0);

            verts[mVBOsize + 0] += Vector4f(static_cast<float>(x), 0, static_cast<float>(z), 1);
            verts[mVBOsize + 1] += Vector4f(static_cast<float>(x), 0, static_cast<float>(z), 1);
            verts[mVBOsize + 2] += Vector4f(static_cast<float>(x), 0, static_cast<float>(z), 1);
            mVBOsize += 3;
         }
      }
   }
   glBindBuffer(GL_ARRAY_BUFFER, mVBO);
   glBufferData(GL_ARRAY_BUFFER, mVBOsize * sizeof(Vector4f), verts, GL_STATIC_DRAW);
   delete[] verts;
}

void PathVisualiser::Tick(TickParameters& tp)
{
   if(mInvalidationWatcher.CheckInvalidation(*mWorldBlocks))
   {
      mPotentialMap.Evaluate();
      BuildMap();
   }

   tp.draw_list.AddToLayer(DrawLayer::Game, this);
   //TODO invalidate on change
}

void PathVisualiser::Draw(TickParameters& tp)
{
   /* Set shader,
      bind VBO with vertices in
      Set attribute point of position input to currently bound buffer (the vbo, with zero offset and stride)
      Connect the position input to the VBO (always the VBO bound to GL_ARRAY_BUFFER)
      */
   mEffect->UseEffect();
   //Set camera
   Matrix4f vpm = tp.camera->GetViewProjection();
   glUniformMatrix4fv(mTransformUniformHandle, 1, false, (float*)&vpm);

   glEnableVertexAttribArray(mPositionAttributeHandle);
 
   glBindBuffer(GL_ARRAY_BUFFER, mVBO);
   glVertexAttribPointer(mPositionAttributeHandle, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4f), 0);
   glDrawArrays(GL_TRIANGLES, 0, mVBOsize);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PathVisualiser::TeardownGraphics(TickParameters& tp)
{
   glDeleteBuffers(1, &mVBO);
   //Effect should be torn down by cache
   tp.draw_list.RemoveFromLayer(DrawLayer::Game, this);
}
