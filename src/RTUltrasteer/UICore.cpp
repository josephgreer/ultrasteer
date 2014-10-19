#include <QObject>
#include "UICore.h"

namespace Nf
{
  ParameterCollection::ParameterCollection(const char *name)
  {
    strcpy(m_name, name);
  }

  
  const char * ParameterCollection::GetName()
  {
    return m_name;
  }

  void ParameterCollection::AddFloatParameter(std::tr1::shared_ptr < FloatParameter > floatParam)
  {
    m_floats.push_back(floatParam);
  }

  void ParameterCollection::AddIntParameter(std::tr1::shared_ptr < IntParameter > intParam)
  {
    m_ints.push_back(intParam);
  }

  void ParameterCollection::AddStringParameter(std::tr1::shared_ptr < StringParameter > stringParam)
  {
    m_strings.push_back(stringParam);
  }
  
  void ParameterCollection::AddBoolParameter(std::tr1::shared_ptr < BoolParameter > boolParam)
  {
    m_bools.push_back(boolParam);
  }
  
  void ParameterCollection::AddActionParameter(std::tr1::shared_ptr < BoolParameter > actionParam)
  {
    m_actions.push_back(actionParam);
  }

  void ParameterCollection::AddEnumParameter(std::tr1::shared_ptr < EnumParameter > enumParam)
  {
    m_enums.push_back(enumParam);
  }

  void ParameterCollection::AddChildCollection(ParameterCollection *child)
  {
    m_children.push_back(child);
  }

  std::vector < std::tr1::shared_ptr < FloatParameter > > ParameterCollection::GetFloatParameters()
  {
    return m_floats;
  }

  std::vector < std::tr1::shared_ptr < IntParameter > > ParameterCollection::GetIntParameters()
  {
    return m_ints;
  }

  std::vector < std::tr1::shared_ptr < StringParameter > > ParameterCollection::GetStringParameters()
  {
    return m_strings;
  }

  std::vector < std::tr1::shared_ptr < BoolParameter > > ParameterCollection::GetBoolParameters()
  {
    return m_bools;
  }

  std::vector < std::tr1::shared_ptr < EnumParameter > > ParameterCollection::GetEnumParameters()
  {
    return m_enums;
  }

  std::vector < std::tr1::shared_ptr < BoolParameter > > ParameterCollection::GetActionParameters()
  {
    return m_actions;
  }


  std::vector < ParameterCollection * > ParameterCollection::GetChildCollections()
  {
    return m_children;
  }
}