#pragma once

#include <string>
#include <vector>

#include "SegmentCore.h"

namespace Nf
{
  typedef void (*Function)(void *); 

  typedef enum {
    PT_BOOL,
    PT_INT,
    PT_FLOAT,
    PT_STRING,
  } PARAMETER_TYPE;

  template <class T>
  class UIElement
  {
  public:
    virtual T GetValue() = 0;
  };

  template <class T>
  class Parameter
  {
  protected:
    T m_val;
    PARAMETER_TYPE m_type;
    char m_name[200];
    std::tr1::shared_ptr < UIElement < T > > m_element;
    Function m_callback;
    void *m_context;

  public:
    Parameter()
    {
      m_context = NULL;
      m_callback = NULL;
    }

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

    virtual void SetUIElement(std::tr1::shared_ptr < UIElement < T > > element)
    {
      m_element = element;
    }

    virtual const char * GetName()
    {
      return m_name;
    }

    virtual Function GetCallback()
    {
      return m_callback;
    }

    virtual void *GetContext()
    {
      return m_context;
    }
  };

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

  typedef NumberParameter < f32 > FloatParameter;
  typedef NumberParameter < s32 > IntParameter;
  typedef Parameter < std::string > StringParameter;
  typedef Parameter < bool > BoolParameter;
  
  class ParameterCollection;

  class ParameterCollection
   {
   protected:
     std::vector < std::tr1::shared_ptr < FloatParameter > > m_floats;
     std::vector < std::tr1::shared_ptr < IntParameter > > m_ints;
     std::vector < std::tr1::shared_ptr < StringParameter > > m_strings;
     std::vector < std::tr1::shared_ptr < BoolParameter > > m_bools;
     std::vector < ParameterCollection * > m_children;

     char m_name[100];

   public:
     ParameterCollection(const char *name);

     const char * GetName();

     void AddFloatParameter(std::tr1::shared_ptr < FloatParameter >  floatParam);
     void AddIntParameter(std::tr1::shared_ptr < IntParameter > intParam);
     void AddStringParameter(std::tr1::shared_ptr < StringParameter > stringParam);
     void AddBoolParameter(std::tr1::shared_ptr < BoolParameter >  boolParam);
     void AddChildCollection(ParameterCollection *child);

     std::vector < std::tr1::shared_ptr < FloatParameter > > GetFloatParameters();
     std::vector < std::tr1::shared_ptr < IntParameter > > GetIntParameters();
     std::vector < std::tr1::shared_ptr < StringParameter > > GetStringParameters();
     std::vector < std::tr1::shared_ptr < BoolParameter > > GetBoolParameters();
     std::vector < ParameterCollection * > GetChildCollections();
   };
}

#define ADD_STRING_PARAMETER(var, name, callback, context, val) \
  var = std::tr1::shared_ptr < Nf::StringParameter > (new Nf::StringParameter(PT_STRING, name, callback, context, val)); \
  m_bools.push_back(var);

#define ADD_INT_PARAMETER(var, name, callback, context, val, min, max, step) \
  var = std::tr1::shared_ptr < Nf::IntParameter > (new Nf::IntParameter(PT_INT, name, callback, context, val, min, max, step)); \
  m_ints.push_back(var);

#define ADD_FLOAT_PARAMETER(var, name, callback, context, val, min, max, step) \
  var = std::tr1::shared_ptr < Nf::FloatParameter > (new Nf::FloatParameter(PT_FLOAT, name, callback, context, val, min, max, step)); \
  m_floats.push_back(var);

#define ADD_BOOL_PARAMETER(var, name, callback, context, val) \
  var = std::tr1::shared_ptr < Nf::BoolParameter > (new Nf::BoolParameter(PT_BOOL, name, callback, context, val)); \
  m_bools.push_back(var);

#define CLASS_CALLBACK(x, type) \
  static void stat##x(void *context) { \
    type * obj = (type *)context; \
    obj->x(); \
  }\

#define CALLBACK_POINTER(x, type) (&type::stat##x)