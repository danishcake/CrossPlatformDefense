#pragma once

#include <GLES2/gl2.h>
#include <map>
#include <string>
#include "../../RefCnt.h"

class Effect;
struct TickParameters;

typedef RefCnt<Effect> EffectPtr;

namespace EffectClearLevel
{
   enum Enum
   {
      None, ClearUnused, ClearAll, Teardown, Initialise
   };
}

/*
 * Effect is a vertex/pixel shader pair
 * Can be subclassed to add getters/setters for particular variables
 */
class Effect
{
private:
   GLuint mVertexShader;
   GLuint mFragmentShader;
   GLuint mShaderProgram;
   const std::string mVertexFile;
   const std::string mFragmentFile;
   bool mInitialised;

   GLuint CompileShader(TickParameters& tp, GLenum shader_type, std::string file);
   GLboolean CheckStatus(GLenum check, GLuint handle, std::string file);
   //Private - use GetEffect
   Effect(std::string vertex_shader, std::string fragment_shader);

   static std::map<std::string, EffectPtr> sCache;

public:
   static EffectPtr GetEffect(std::string vertex_shader, std::string fragment_shader);
   static void CacheTick(TickParameters& tp, EffectClearLevel::Enum clear_level);

   virtual void InitialiseGraphics(TickParameters& tp);
   virtual void TeardownGraphics(TickParameters& tp);
   virtual void UseEffect();
   virtual void StopUsingEffect();
   GLint GetAttributeLocation(const char* attribute);
   GLint GetUniformLocation(const char* uniform);
};
