#include "Effect.h"
#include "../../Log.h"
#include "../../CheckMacros.h"
#include "../../TickParameters.h"

std::map<std::string, EffectPtr> Effect::sCache;
EffectPtr Effect::GetEffect(std::string vertex_shader, std::string fragment_shader)
{
   std::string key = vertex_shader + "/" + fragment_shader;
   std::map<std::string, EffectPtr>::iterator fx_it = sCache.find(key);
   if(fx_it != sCache.end())
   {
      return fx_it->second;
   } else
   {
      EffectPtr fx_ptr = EffectPtr(new Effect(vertex_shader, fragment_shader));
      sCache[key] = fx_ptr;
      return fx_ptr;
   }
}

void Effect::CacheTick(TickParameters& tp, EffectClearLevel::Enum clear_level)
{
   if(clear_level == EffectClearLevel::None)
      return;

   //Teardown effects if Teardown passed in
   if(clear_level == EffectClearLevel::Teardown)
   {
      for(std::map<std::string, EffectPtr>::iterator it = sCache.begin(); it != sCache.end(); ++it)
      {
         it->second->TeardownGraphics(tp);
      }
      return;
   }

   //Initialise effects if Initialise passed in
   if(clear_level == EffectClearLevel::Initialise)
   {
      for(std::map<std::string, EffectPtr>::iterator it = sCache.begin(); it != sCache.end(); ++it)
      {
         it->second->InitialiseGraphics(tp);
      }
      return;
   }

   //Clear unused effects if ClearUnused passed in
   for(std::map<std::string, EffectPtr>::iterator it = sCache.begin(); it != sCache.end(); ++it)
   {
      if(clear_level == EffectClearLevel::ClearAll ||
         (clear_level == EffectClearLevel::ClearUnused && it->second.use_count() == 1))
      {
         it->second->TeardownGraphics(tp);
      }
   }
   //Will be ClearAll or ClearUnused by here. Only safe to remove items from the list if use_count is 1
   //Otherwise could resurrected
   std::map<std::string, EffectPtr>::iterator it = sCache.begin();
   while(it != sCache.end())
   {
      if(it->second.use_count() == 1)
      {
         sCache.erase(it++);
      } else
      {
         ++it;
      }
   }
}

Effect::Effect(std::string vertex_shader, std::string fragment_shader)
   : mVertexFile(vertex_shader),
     mFragmentFile(fragment_shader),
     mInitialised(false)
{
}

//TODO move this to somewhere else
GLuint Effect::CompileShader(TickParameters& tp, GLenum shader_type, std::string file)
{
   //Load source into a file
   void* file_hdl = tp.file->Open(file.c_str());
   int data_size;
   const void* data = tp.file->ReadAll(file_hdl, &data_size);

   GL_CHECK(GLuint shader = glCreateShader(shader_type));
   GL_CHECK(glShaderSource(shader, 1, (const char**)&data, &data_size));
   GL_CHECK(glCompileShader(shader));
   if(data == NULL)
   {
      Log::Error("Effect", "File not found: %s", file.c_str());
   }

   if(file_hdl == NULL || data == NULL || CheckStatus(GL_COMPILE_STATUS, shader, file))
   {
      glDeleteShader(shader);
      shader = 0;
   }
   //Close file
   tp.file->Close(file_hdl);
   return shader;
}

//Returns 0 if last shader action went OK
GLboolean Effect::CheckStatus(GLenum check, GLuint handle, std::string file)
{
   GLint compileSuccess;

   if(check == GL_LINK_STATUS)
      glGetProgramiv(handle, check, &compileSuccess);
   else
      glGetShaderiv(handle, check, &compileSuccess);
   if (compileSuccess == GL_FALSE) {
      char messages[256];
      if(check == GL_LINK_STATUS)
         glGetProgramInfoLog(handle, sizeof(messages), 0, &messages[0]);
      else
         glGetShaderInfoLog(handle, sizeof(messages), 0, &messages[0]);
      messages[255] = '\0';
      if(check == GL_LINK_STATUS)
         Log::Error("Shader link:", "Error: %s", &messages[0]);
      else
         Log::Error("Shader compile:", "File: %s Error: %s", file.c_str(), &messages[0]);
      return 1;
   }
   return 0;
}

void Effect::InitialiseGraphics(TickParameters& tp)
{
   if(mInitialised)
      return;
   //First compile and link vertex and fragment shaders
   mVertexShader = CompileShader(tp, GL_VERTEX_SHADER, mVertexFile);
   if(!mVertexShader)
   {
      Log::Error(__FILE__, "Vertex shader not created, Effect will not work");
      return;
   }
   mFragmentShader = CompileShader(tp, GL_FRAGMENT_SHADER, mFragmentFile);
   if(!mFragmentShader)
   {
      Log::Error(__FILE__, "Fragment shader not created, Effect will not work");
      return;
   }

   //Then create vertex buffer object and populate
   mShaderProgram = glCreateProgram();
   
   GL_CHECK(glAttachShader(mShaderProgram, mVertexShader));
   GL_CHECK(glAttachShader(mShaderProgram, mFragmentShader));
   GL_CHECK(glLinkProgram(mShaderProgram));

   if(CheckStatus(GL_LINK_STATUS, mShaderProgram, "link"))
   {
      glDeleteProgram(mShaderProgram);
      mShaderProgram = 0;
   }
   if(mVertexShader != 0)
      glDeleteShader(mVertexShader);
   if(mFragmentShader != 0)
      glDeleteShader(mFragmentShader);
   mVertexShader = 0;
   mFragmentShader = 0;

   //Diagnostic output:

   std::map<GLenum, const char*> types;
   types[GL_FLOAT] = "GL_FLOAT";
   types[GL_FLOAT_VEC2] = "GL_FLOAT_VEC2";
   types[GL_FLOAT_VEC3] = "GL_FLOAT_VEC3";
   types[GL_FLOAT_VEC4] = "GL_FLOAT_VEC4";
   types[GL_FLOAT_MAT2] = "GL_FLOAT_MAT2";
   types[GL_FLOAT_MAT3] = "GL_FLOAT_MAT3";
   types[GL_FLOAT_MAT4] = "GL_FLOAT_MAT4";

   GLint attrib_count;
   GLint max_attrib_length;
   GL_CHECK(glGetProgramiv(mShaderProgram, GL_ACTIVE_ATTRIBUTES, &attrib_count));
   GL_CHECK(glGetProgramiv(mShaderProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_attrib_length));

   Log::Debug("Effect::InitialiseGraphics", "Effect built from %s and %s", mVertexFile.c_str(), mFragmentFile.c_str());
   Log::Debug("Effect::InitialiseGraphics", "Attribute count: %d", attrib_count);
   char* data = new char[max_attrib_length+1];
   for(int i = 0; i < attrib_count; i++)
   {
      GLint size;
      GLenum type;
      GL_CHECK(glGetActiveAttrib(mShaderProgram, i, max_attrib_length, 0, &size, &type, data));
      const char* type_string = "Unknown type";
      if(types.find(type) != types.end())
      {
         type_string = types[type];
      }

      if(size > 1)
         Log::Debug("Effect::InitialiseGraphics", "%s %s[%d];", type_string, data, size);
      else
         Log::Debug("Effect::InitialiseGraphics", "%s %s;", type_string, data);
   }
   delete[] data;


   if(mShaderProgram)
      mInitialised = true;
}

void Effect::TeardownGraphics(TickParameters& tp)
{
   glDeleteProgram(mShaderProgram);
   mShaderProgram = 0;
   mInitialised = false;
}

void Effect::UseEffect()
{
   glUseProgram(mShaderProgram);
}

void Effect::StopUsingEffect()
{
   glUseProgram(0);
}

GLint Effect::GetAttributeLocation(const char* attribute)
{
   GLint location = glGetAttribLocation(mShaderProgram, attribute);
   if(location == -1)
   {
      Log::Error(__FILE__, "Unable to find attribute %s in shader built from %s and %s", 
                 attribute, mVertexFile.c_str(), mFragmentFile.c_str());
   }
   return location;
}

GLint Effect::GetUniformLocation(const char* uniform)
{
   GLint location = glGetUniformLocation(mShaderProgram, uniform);
   if(location == -1)
   {
      Log::Error(__FILE__, "Unable to find uniform %s in shader built from %s and %s", 
                 uniform, mVertexFile.c_str(), mFragmentFile.c_str());
   }
   return location;
}
