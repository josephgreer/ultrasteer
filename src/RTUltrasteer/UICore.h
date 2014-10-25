#pragma once

#include <string>
#include <vector>

#include "SegmentCore.h"

#ifdef QT_GUI_LIB
#include <QObject>
#endif

#define VIS_WIDTH 600
#define VIS_HEIGHT 480

namespace Nf
{
  //Callback type for UI events
  typedef void (*Function)(void *); 

  typedef enum {
    PT_BOOL,
    PT_INT,
    PT_FLOAT,
    PT_STRING,
    PT_OPEN_FILE,
    PT_SAVE_FILE,
    PT_ACTION,
    PT_ENUM,
    PT_VEC3D,
    PT_VEC3F,
    PT_VEC3I,
    PT_VEC2D,
    PT_VEC2F,
    PT_VEC2I,
  } PARAMETER_TYPE;

  //UIElement is an interface that is impelmented in QT projects
  //UIElement is defined abstractly to make it so that projects
  //can implement paramters without having to be QT projects and 
  //include QT headers.  A concrete UIElement is defined in 
  //RTUltrasteer.cpp
  template <class T>
  class UIElement
  {
  public:
    virtual T GetValue() = 0;  //Get the UI element's value
  };

  template <class T>
  class NumberUIElement : public UIElement<T>
  {
  public:
    virtual void SetMin(T min) = 0;
    virtual void SetMax(T min) = 0;
    virtual void SetStep(T min) = 0;
  };

  //Basic parameter class (T = string, float, int, etc.) 
  template <class T>
  class Parameter
  {
  protected:
    T m_val;                            //Current value
    PARAMETER_TYPE m_type;              //Type
    char m_name[200];                   //String that is displayed to user
    std::tr1::shared_ptr < UIElement < T > > m_element;   //Pointer to UI Element that represents the paramter.  This is set later by the code that displays this parameter
    Function m_callback;                //Callback function for UI interaction with this parameter.  This is set at creation time.
    void *m_context;                    //Callback context for UI interaction.  (Usually a class pointer.)  This is set at creation time.

  public:
    Parameter()
    {
      m_context = NULL;
      m_callback = NULL;
    }

    //Constructor for generic parameter
    //Definition is self-explanmatory.  
    //Parameters are generally created with the ADD_xx_PARAMETER macros below
    Parameter(PARAMETER_TYPE type, const char *name, Function callback, void *context, T def)
      : m_val(def)
      , m_type(type)
      , m_callback(callback)
      , m_context(context)
    {
      strcpy(m_name, name);
    }

    virtual T GetValue()
    {
      if(m_element)
        m_val = m_element->GetValue();
      return m_val;
    }

    //Sets the UIElement class.  This is set in GUI code that actually creates
    //a UI manifestation of this element
    virtual void SetUIElement(std::tr1::shared_ptr < UIElement < T > > element)
    {
      m_element = element;
    }

    virtual const char * GetName()
    {
      return m_name;
    }

    //Used in GUI code that creates a UI manifestation of the parameter
    virtual Function GetCallback()
    {
      return m_callback;
    }


    //Used in GUI code that creates a UI manifestation of the parameter
    virtual void *GetContext()
    {
      return m_context;
    }
  };

  //Number parameter class (T = float, int, etc.) 
  //Same as Parameter, only min, max, and step can be specified
  template < class T >
  class NumberParameter : public Parameter < T >
  {
  protected:
    T m_min;
    T m_max;
    T m_step;

  public:
    NumberParameter()
      : Parameter()
    {
    }

    NumberParameter(PARAMETER_TYPE type, const char *name, Function callback, void *context, T def, T min, T max, T step)
      : Parameter(type, name, callback, context, def)
      , m_min(min)
      , m_max(max)
      , m_step(step)
    {

    }

    T GetMin()
    {
      return m_min;
    }

    T GetMax()
    {
      return m_max;
    }

    T GetStep()
    {
      return m_step;
    }

    void SetMin(T min)
    {
      NumberUIElement<T> * nui = (NumberUIElement<T> *)(&(*m_element));
      nui->SetMin(min);
    }

    void SetMax(T min)
    {
      NumberUIElement<T> * nui = (NumberUIElement<T> *)(&(*m_element));
      nui->SetMax(min);
    }

    void SetStep(T min)
    {
      NumberUIElement<T> * nui = (NumberUIElement<T> *)(&(*m_element));
      nui->SetStep(min);
    }
  };

  //File parameter class 
  class FileParameter : public Parameter < std::string >
  {
  protected:
    std::string m_extWC;        //extension wildcard
  
  public:
    FileParameter()
      : Parameter()
    {
    }

    FileParameter(PARAMETER_TYPE type, const char *name, Function callback, void *context, std::string def, std::string extWC)
      : Parameter(type, name, callback, context, def)
      , m_extWC(extWC)
    {
    }

    std::string GetDirectory()
    {
      return m_val.substr(0,m_val.find_last_of("/"));
    }

    std::string GetFilename()
    {
      return m_val.substr(m_val.find_last_of("/")+1,std::string::npos);
    }

    std::string GetExtensionWildcard()
    {
      return m_extWC.c_str();
    }
  };    

  //################################BEGIN ENUM DEFINITIONS#############################################
#ifdef QT_GUI_LIB
  class QtEnums : public QObject
  {
    Q_OBJECT

    Q_ENUMS(VisRenderMethod)
    Q_ENUMS(VOLUME_ORIGIN_LOCATION)
#else
  class QtEnums
  {
#endif
  public:
  enum VisRenderMethod 
  {
    Texture_2D = 0,
    Texture_3D,
    RayCastingMIP,
  };

  //used for specifying origin location when setting a volume from a base frame
  enum VOLUME_ORIGIN_LOCATION
  {
    VOL_LEFT = 0,
    VOL_RIGHT,
    VOL_QUARTER_LEFT,
    VOL_QUARTER_RIGHT,
    VOL_MIDDLE,
  };

  };

  //################################END ENUM DEFINITIONS###############################################
  
  //Enumerable parameter class 
  //Same as Parameter, only corresponding string list can be supplied
  //Enum class takes advantage of QT introspection to convert between
  //enums and strings.  Therefore the enum has to be defined in a Q_OBJECT class    
  class EnumParameter : public Parameter < s32 >
  {
  protected:
    std::string m_enumName;

  public:
    EnumParameter()
      : Parameter()
    {
    }

    EnumParameter(PARAMETER_TYPE type, const char *name, Function callback, void *context, s32 def, const char *enumName)
      : Parameter(type, name, callback, context, def)
    {
      m_enumName = enumName;
    }

    const char * GetEnumName() { return m_enumName.c_str(); }
  };


  typedef NumberParameter < f32 > FloatParameter;
  typedef NumberParameter < s32 > IntParameter;
  typedef NumberParameter < Vec3d > Vec3dParameter;
  typedef NumberParameter < Vec3f > Vec3fParameter;
  typedef NumberParameter < Vec3i > Vec3iParameter;
  typedef NumberParameter < Vec2d > Vec2dParameter;
  typedef NumberParameter < Vec2f > Vec2fParameter;
  typedef NumberParameter < Vec2i > Vec2iParameter;
  typedef Parameter < std::string > StringParameter;
  typedef Parameter < bool > BoolParameter;
  
  class ParameterCollection;

  //A parameter collection is a container of parameters
  //A UI manifestation of a parameter collection is a
  //dropdown in a tree view.
  //Parameters themselves are tree leaves.
  class ParameterCollection
   {
   protected:
     std::vector < std::tr1::shared_ptr < FloatParameter > > m_floats;
     std::vector < std::tr1::shared_ptr < IntParameter > > m_ints;
     std::vector < std::tr1::shared_ptr < StringParameter > > m_strings;
     std::vector < std::tr1::shared_ptr < FileParameter > > m_ofiles;;
     std::vector < std::tr1::shared_ptr < FileParameter > > m_sfiles;
     std::vector < std::tr1::shared_ptr < BoolParameter > > m_bools;
     std::vector < std::tr1::shared_ptr < BoolParameter > > m_actions;
     std::vector < std::tr1::shared_ptr < EnumParameter > > m_enums;
     std::vector < std::tr1::shared_ptr < Vec3dParameter > > m_vec3ds;
     std::vector < std::tr1::shared_ptr < Vec3fParameter > > m_vec3fs;
     std::vector < std::tr1::shared_ptr < Vec3iParameter > > m_vec3is;
     std::vector < std::tr1::shared_ptr < Vec2dParameter > > m_vec2ds;
     std::vector < std::tr1::shared_ptr < Vec2fParameter > > m_vec2fs;
     std::vector < std::tr1::shared_ptr < Vec2iParameter > > m_vec2is;
     std::vector < ParameterCollection * > m_children;

     char m_name[100];

   public:
     ParameterCollection(const char *name);

     const char * GetName();

     void AddFloatParameter(std::tr1::shared_ptr < FloatParameter >  floatParam);
     void AddIntParameter(std::tr1::shared_ptr < IntParameter > intParam);
     void AddStringParameter(std::tr1::shared_ptr < StringParameter > stringParam);
     void AddSaveFileParameter(std::tr1::shared_ptr < FileParameter > ofile);
     void AddOpenFileParameter(std::tr1::shared_ptr < FileParameter > sfile);
     void AddBoolParameter(std::tr1::shared_ptr < BoolParameter >  boolParam);
     void AddActionParameter(std::tr1::shared_ptr < BoolParameter >  actionParam);
     void AddEnumParameter(std::tr1::shared_ptr < EnumParameter > enumParam);
     void AddVec3dParameter(std::tr1::shared_ptr < Vec3dParameter > vec3dParam);
     void AddVec3fParameter(std::tr1::shared_ptr < Vec3fParameter > vec3fParam);
     void AddVec3iParameter(std::tr1::shared_ptr < Vec3iParameter > vec3iParam);
     void AddVec2dParameter(std::tr1::shared_ptr < Vec2dParameter > vec2dParam);
     void AddVec2fParameter(std::tr1::shared_ptr < Vec2fParameter > vec2fParam);
     void AddVec2iParameter(std::tr1::shared_ptr < Vec2iParameter > vec2iParam);
     void AddChildCollection(ParameterCollection *child);

     std::vector < std::tr1::shared_ptr < FloatParameter > > GetFloatParameters();
     std::vector < std::tr1::shared_ptr < IntParameter > > GetIntParameters();
     std::vector < std::tr1::shared_ptr < StringParameter > > GetStringParameters();
     std::vector < std::tr1::shared_ptr < FileParameter > > GetOpenFileParameters();
     std::vector < std::tr1::shared_ptr < FileParameter > > GetSaveFileParameters();
     std::vector < std::tr1::shared_ptr < BoolParameter > > GetBoolParameters();
     std::vector < std::tr1::shared_ptr < BoolParameter > > GetActionParameters();
     std::vector < std::tr1::shared_ptr < EnumParameter > > GetEnumParameters();
     std::vector < std::tr1::shared_ptr < Vec3dParameter > > GetVec3dParameters();
     std::vector < std::tr1::shared_ptr < Vec3fParameter > > GetVec3fParameters();
     std::vector < std::tr1::shared_ptr < Vec3iParameter > > GetVec3iParameters();
     std::vector < std::tr1::shared_ptr < Vec2dParameter > > GetVec2dParameters();
     std::vector < std::tr1::shared_ptr < Vec2fParameter > > GetVec2fParameters();
     std::vector < std::tr1::shared_ptr < Vec2iParameter > > GetVec2iParameters();
     std::vector < ParameterCollection * > GetChildCollections();
   };
}

//These macros are used in a cpp file of a ParameterCollection
//They take care of creaitng the parameter and inserting them into the collection
#define ADD_STRING_PARAMETER(var, name, callback, context, val) \
  var = std::tr1::shared_ptr < Nf::StringParameter > (new Nf::StringParameter(PT_STRING, name, callback, context, val)); \
  m_strings.push_back(var);

#define ADD_OPEN_FILE_PARAMETER(var, name, callback, context, val, extension) \
  var = std::tr1::shared_ptr < Nf::FileParameter > (new Nf::FileParameter(PT_OPEN_FILE, name, callback, context, val, extension)); \
  m_ofiles.push_back(var);

#define ADD_SAVE_FILE_PARAMETER(var, name, callback, context, val, extension) \
  var = std::tr1::shared_ptr < Nf::FileParameter > (new Nf::FileParameter(PT_SAVE_FILE, name, callback, context, val, extension)); \
  m_sfiles.push_back(var);

#define ADD_INT_PARAMETER(var, name, callback, context, val, min, max, step) \
  var = std::tr1::shared_ptr < Nf::IntParameter > (new Nf::IntParameter(PT_INT, name, callback, context, val, min, max, step)); \
  m_ints.push_back(var);

#define ADD_FLOAT_PARAMETER(var, name, callback, context, val, min, max, step) \
  var = std::tr1::shared_ptr < Nf::FloatParameter > (new Nf::FloatParameter(PT_FLOAT, name, callback, context, val, min, max, step)); \
  m_floats.push_back(var);

#define ADD_BOOL_PARAMETER(var, name, callback, context, val) \
  var = std::tr1::shared_ptr < Nf::BoolParameter > (new Nf::BoolParameter(PT_BOOL, name, callback, context, val)); \
  m_bools.push_back(var);

#define ADD_ACTION_PARAMETER(var, name, callback, context, val) \
  var = std::tr1::shared_ptr < Nf::BoolParameter > (new Nf::BoolParameter(PT_ACTION, name, callback, context, val)); \
  m_actions.push_back(var);

#define ADD_ENUM_PARAMETER(var, name, callback, context, val, enumName) \
  var = std::tr1::shared_ptr < Nf::EnumParameter > (new Nf::EnumParameter(PT_ENUM, name, callback, context, val, enumName)); \
  m_enums.push_back(var);

#define ADD_VEC3D_PARAMETER(var, name, callback, context, val, min, max, step) \
  var = std::tr1::shared_ptr < Nf::Vec3dParameter > (new Nf::Vec3dParameter(PT_VEC3D, name, callback, context, val, min, max, step)); \
  m_vec3ds.push_back(var);

#define ADD_VEC3F_PARAMETER(var, name, callback, context, val, min, max, step) \
  var = std::tr1::shared_ptr < Nf::Vec3fParameter > (new Nf::Vec3fParameter(PT_VEC3F, name, callback, context, val, min, max, step)); \
  m_vec3fs.push_back(var);

#define ADD_VEC3I_PARAMETER(var, name, callback, context, val, min, max, step) \
  var = std::tr1::shared_ptr < Nf::Vec3iParameter > (new Nf::Vec3iParameter(PT_VEC3I, name, callback, context, val, min, max, step)); \
  m_vec3is.push_back(var);

#define ADD_VEC2D_PARAMETER(var, name, callback, context, val, min, max, step) \
  var = std::tr1::shared_ptr < Nf::Vec2dParameter > (new Nf::Vec2dParameter(PT_VEC2D, name, callback, context, val, min, max, step)); \
  m_vec2ds.push_back(var);

#define ADD_VEC2F_PARAMETER(var, name, callback, context, val, min, max, step) \
  var = std::tr1::shared_ptr < Nf::Vec2fParameter > (new Nf::Vec2fParameter(PT_VEC2F, name, callback, context, val, min, max, step)); \
  m_vec2fs.push_back(var);

#define ADD_VEC2I_PARAMETER(var, name, callback, context, val, min, max, step) \
  var = std::tr1::shared_ptr < Nf::Vec2iParameter > (new Nf::Vec2iParameter(PT_VEC2I, name, callback, context, val, min, max, step)); \
  m_vec2is.push_back(var);

#define ADD_CHILD_COLLECTION(var) \
  m_children.push_back((Nf::ParameterCollection *)var);

//Since calbacks must be static and we usually want to call a class function
//a class, this takes care of the dirty work of creating a static function,
//and calling a class function of the same name and then calling the class function
//on the context paramter
#define CLASS_CALLBACK(x, type) \
  static void stat##x(void *context) { \
    type * obj = (type *)context; \
    obj->x(); \
  }\

//When we need to pass a callback to an ADD_xx_PARAMETER macro,
//we use this to get a reference to the callback.  The same arguments
//should be used as the CLASS_CALLBACK macro
#define CALLBACK_POINTER(x, type) (&type::stat##x)

//Example:
//I have a float parameter, FloatParameter m_gravity;
//I want an MyClass::onGravityChanged() function to be called
//I can't use a member function as a callback function pointer 
//Therefore I create a static function static MyClass::statOnGravityChanged(void *context)
//and in it i call ((MyClass *)(context))->onGravityChanged();
//To save the time of writing this code for every parameter, I just use the following lines of code:
// class MyClass : ParameterCollection {
//   FloatParameter m_gravity;
//    void onGravityChanged();
//    CLASS_CALLBACK(onGravityChanged, MyClass);
// }

// In the MyClass Constructor I do the following:
// MyClass::MyClass() {
//    ADD_FLOAT_PARAMETER(m_gravity, "Gravity", CALLBACK_POINTER(onGravityChanged, MyClass), this, 9.8f);
//}