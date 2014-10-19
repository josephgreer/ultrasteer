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
    PT_ACTION,
    PT_ENUM,
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
  };  

  //################################BEGIN ENUM DEFINITIONS#############################################
#ifdef QT_GUI_LIB
  class QtEnums : public QObject
  {
    Q_OBJECT

    Q_ENUMS(VisRenderMethod)
  public:
#else
  class QtEnums
  {
#endif
  enum VisRenderMethod 
  {
    Texture_2D = 0,
    Texture_3D,
    RayCastingMIP,
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
     std::vector < std::tr1::shared_ptr < BoolParameter > > m_bools;
     std::vector < std::tr1::shared_ptr < BoolParameter > > m_actions;
     std::vector < std::tr1::shared_ptr < EnumParameter > > m_enums;
     std::vector < ParameterCollection * > m_children;

     char m_name[100];

   public:
     ParameterCollection(const char *name);

     const char * GetName();

     void AddFloatParameter(std::tr1::shared_ptr < FloatParameter >  floatParam);
     void AddIntParameter(std::tr1::shared_ptr < IntParameter > intParam);
     void AddStringParameter(std::tr1::shared_ptr < StringParameter > stringParam);
     void AddBoolParameter(std::tr1::shared_ptr < BoolParameter >  boolParam);
     void AddActionParameter(std::tr1::shared_ptr < BoolParameter >  actionParam);
     void AddEnumParameter(std::tr1::shared_ptr < EnumParameter > enumParam);
     void AddChildCollection(ParameterCollection *child);

     std::vector < std::tr1::shared_ptr < FloatParameter > > GetFloatParameters();
     std::vector < std::tr1::shared_ptr < IntParameter > > GetIntParameters();
     std::vector < std::tr1::shared_ptr < StringParameter > > GetStringParameters();
     std::vector < std::tr1::shared_ptr < BoolParameter > > GetBoolParameters();
     std::vector < std::tr1::shared_ptr < BoolParameter > > GetActionParameters();
     std::vector < std::tr1::shared_ptr < EnumParameter > > GetEnumParameters();
     std::vector < ParameterCollection * > GetChildCollections();
   };
}

//These macros are used in a cpp file of a ParameterCollection
//They take care of creaitng the parameter and inserting them into the collection
#define ADD_STRING_PARAMETER(var, name, callback, context, val) \
  var = std::tr1::shared_ptr < Nf::StringParameter > (new Nf::StringParameter(PT_STRING, name, callback, context, val)); \
  m_strings.push_back(var);

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

#define ADD_CHILD_COLLECTION(var) \
  m_children.push_back((Nf::ParameterCollection *)&var);

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