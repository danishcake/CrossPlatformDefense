#include "WorldDrawer.h"
#include "../../Log.h"
#include "../../WorldBlocks.h"
#include "../../Camera.h"

struct WorldVertex
{
   Vector4f pos;
   Vector2f texc;
};

WorldDrawer::WorldDrawer(WorldBlocks* blocks)
 : mWorldBlocks(blocks),
   mEffect(Effect::GetEffect("glsl/WorldVertex.glsl", "glsl/WorldFragment.glsl")),
   mTexture(Texture::GetTexture("textures/TerrainTexture.png"))
{
}

void WorldDrawer::buildColumn(int x, int z, int vbo_index)
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
   const float tx1 = 0.125f - txe;
   const float ty0 = 0 + txe;
   const float ty1 = 0.125f - txe;
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
         t0, t1, t3,   t0, t3, t2,
         t0, t1, t3,   t0, t3, t2
   };

   WorldVertex* verts = new WorldVertex[WORLD_HEIGHT * 36 * COLUMN_SIZE * COLUMN_SIZE];

   int face_index = 0; //Only increment this when a face is added
   for(int sx = 0; sx < COLUMN_SIZE; sx++)
   {
      for(int sz = 0; sz < COLUMN_SIZE; sz++)
      {
         Column column = mWorldBlocks->getColumn(x + sx, z + sz);
         /*    1   x->
          *  0   2
          *    3  
          *  z
          *  \/          
          */
         Column neighbours[4];
         if(x + sx > 0)                                              neighbours[0] = mWorldBlocks->getColumn(x + sx - 1, z + sz);
         if(z + sz > 0)                                              neighbours[1] = mWorldBlocks->getColumn(x + sx,     z + sz - 1);
         if(x + sx < WORLD_WIDTH - 1)                                neighbours[2] = mWorldBlocks->getColumn(x + sx + 1, z + sz);
         if(z + sz < WORLD_BREADTH - 1)                              neighbours[3] = mWorldBlocks->getColumn(x + sx,     z + sz + 1);

         for(int y = 0; y < WORLD_HEIGHT; y++)
         {
            Block& b = column.Get(y);
            if(b.blockType.Value() != BlockType::Empty)
            {
               const int bt = (int)b.blockType.Value() - 1; //Code zero corresponds to empty
               //Create cube faces
               /*
                * First 24 vertices describe sides of cube, then the bottom 6, then the top 6
                * Textures are packed into two rows of 8. Each row has two subrows with side and top
                * packed into them
                * Eg given 256x256 the first texture will be packed into the first 32x64
                */
               const Vector4f v_origin((float)(x + sx), (float)y, (float)(z + sz), 0);
               const Vector2f t_origin_side    ((bt % 8) * 0.125f, (bt / 8) * 0.25f);
               const Vector2f t_origin_side_top((bt % 8) * 0.125f, (bt / 8) * 0.25f + 0.125f);

               /*
                * The majority of the cube faces are not visible, so only add them to the vertex buffer if 
                * the neighbour is empty
                */
               bool face_present[6] = {0};
               //Near X
               if(x + sx == 0                 || neighbours[0].Get(y).blockType.Value() == BlockType::Empty)
                  face_present[0] = true;
               //Far X
               if(x + sx == WORLD_WIDTH - 1   || neighbours[2].Get(y).blockType.Value() == BlockType::Empty)
                  face_present[1] = true;
               //Near Z
               if(z + sz == 0                 || neighbours[1].Get(y).blockType.Value() == BlockType::Empty)
                  face_present[2] = true;
               //Far Z
               if(z + sz == WORLD_BREADTH - 1 || neighbours[3].Get(y).blockType.Value() == BlockType::Empty)
                  face_present[3] = true;

               //Bottom (Near Y)
               if(y == 0 || column.Get(y - 1).blockType.Value() == BlockType::Empty)
                  face_present[4] = true;
               //Top (Far Y)
               if(y == WORLD_HEIGHT - 1 || column.Get(y + 1).blockType.Value() == BlockType::Empty)
                  face_present[5] = true;

               //Sides and bottom
               for(int face = 0; face < 5; face++)
               {
                  if(face_present[face])
                  {
                     for(int i = 0; i < 6; i++)
                     {
                        int read_i = face * 6 + i;
                        verts[face_index * 6 + i].pos  = v_origin + offsets[read_i];
                        verts[face_index * 6 + i].texc = t_origin_side + texc_offsets[read_i];
                     }
                     face_index += 1;
                  }
               }

               //top
               if(face_present[5])
               {
                  for(int i = 0; i < 6; i++)
                  {
                     int read_i = 30 + i;
                     verts[face_index * 6 + i].pos  = v_origin + offsets[read_i];
                     verts[face_index * 6 + i].texc = t_origin_side_top + texc_offsets[read_i];
                  }
                  face_index += 1;
               }
            }
         }
         mVBOsize[vbo_index] = face_index * 6;

         glBindBuffer(GL_ARRAY_BUFFER, mVBO[vbo_index]);
         glBufferData(GL_ARRAY_BUFFER, mVBOsize[vbo_index] * sizeof(WorldVertex), verts, GL_STATIC_DRAW);
      }
   }
   delete[] verts;
}

void WorldDrawer::InitialiseGraphics(TickParameters& tp)
{
   mEffect->InitialiseGraphics(tp);
   mTexture->InitialiseGraphics(tp);

   mPositionAttributeHandle = mEffect->GetAttributeLocation("InputPosition");
   mTexcAttributeHandle = mEffect->GetAttributeLocation("InputTextureCoord");
   mTransformUniformHandle = mEffect->GetUniformLocation("Transform");

   //Initialise VBO
   glGenBuffers(WORLD_WIDTH * WORLD_BREADTH / (COLUMN_SIZE * COLUMN_SIZE), mVBO);

   int triangle_count = 0;

   for(int x = 0; x < WORLD_WIDTH / COLUMN_SIZE; x++)
   {
      for(int z = 0; z < WORLD_BREADTH / COLUMN_SIZE; z++)
      {
         buildColumn(x * COLUMN_SIZE, z * COLUMN_SIZE, x * WORLD_BREADTH / COLUMN_SIZE + z);
         triangle_count += mVBOsize[x * WORLD_BREADTH / COLUMN_SIZE + z] / 3;
      }
   }
   mInvalidationWatcher.CheckInvalidation(*mWorldBlocks);
   Log::Debug("WorldDrawer::InitialiseGraphics", "Built VBOs. Triangle count: %d", triangle_count);
}

void WorldDrawer::Tick(TickParameters& tp)
{
   //Check invalidation
   for(int x = 0; x < WORLD_WIDTH / COLUMN_SIZE; x++)
   {
      for(int z = 0; z < WORLD_BREADTH / COLUMN_SIZE; z++)
      {
         bool invalidated = false;
         for(int sx = 0; sx < COLUMN_SIZE && !invalidated; sx++)
         {
            for(int sz = 0; sz < COLUMN_SIZE && !invalidated; sz++)
            {
               Column column = mWorldBlocks->getColumn(x * COLUMN_SIZE + sx, z * COLUMN_SIZE + sz);
               invalidated |= mInvalidationWatcher.CheckInvalidation(column);
            }
         }
         if(invalidated)
         {
            Log::Debug("WorldDrawer::Tick", "Updating VBO: %d,%d", x, z);
            buildColumn(x * COLUMN_SIZE, z * COLUMN_SIZE, x * WORLD_BREADTH / COLUMN_SIZE + z);
         }
      }
   }
   tp.draw_list.AddToLayer(DrawLayer::Game, this);
}

void WorldDrawer::Draw(TickParameters& tp)
{
   /* Set shader,
      bind VBO with vertices in
      Set attribute point of position input to currently bound buffer (the vbo, with zero offset and stride)
      Connect the position input to the VBO (always the VBO bound to GL_ARRAY_BUFFER)
      */
   mEffect->UseEffect();
   mTexture->UseTexture(0);
   //Set camera
   Matrix4f vpm = tp.camera->GetViewProjection();
   glUniformMatrix4fv(mTransformUniformHandle, 1, false, (float*)&vpm);

   glEnableVertexAttribArray(mPositionAttributeHandle);
   glEnableVertexAttribArray(mTexcAttributeHandle);

   for(int x = 0; x < WORLD_WIDTH / COLUMN_SIZE; x++)
   {
      for(int y = 0; y < WORLD_BREADTH / COLUMN_SIZE; y++)
      {
         glBindBuffer(GL_ARRAY_BUFFER, mVBO[x * WORLD_BREADTH / COLUMN_SIZE + y]);
         glVertexAttribPointer(mPositionAttributeHandle, 4, GL_FLOAT, GL_FALSE, sizeof(WorldVertex), 0);
         glVertexAttribPointer(mTexcAttributeHandle, 2, GL_FLOAT, GL_FALSE, sizeof(WorldVertex), (void*)(4 * sizeof(float)));
         
         glDrawArrays(GL_TRIANGLES, 0, mVBOsize[x * WORLD_BREADTH / COLUMN_SIZE + y]);
      }
   }
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void WorldDrawer::TeardownGraphics(TickParameters& tp)
{
   glDeleteBuffers(WORLD_WIDTH * WORLD_BREADTH, mVBO);
   //Texture and effect should be torn down by cache

   //Clear VBOs
   for(int x = 0; x < WORLD_WIDTH / COLUMN_SIZE; x++)
   {
      for(int z = 0; z < WORLD_BREADTH / COLUMN_SIZE; z++)
      {
         mVBO[x * WORLD_BREADTH / COLUMN_SIZE + z] = 0;
         mVBOsize[x * WORLD_BREADTH / COLUMN_SIZE + z] = 0;
      }
   }

   tp.draw_list.RemoveFromLayer(DrawLayer::Game, this);
}
