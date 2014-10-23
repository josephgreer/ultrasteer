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

  void ParameterCollection::AddOpenFileParameter(std::tr1::shared_ptr < FileParameter > ofile)
  {
    m_ofiles.push_back(ofile);
  }

  void ParameterCollection::AddSaveFileParameter(std::tr1::shared_ptr < FileParameter > sfile)
  {
    m_sfiles.push_back(sfile);
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

  void ParameterCollection::AddVec3dParameter(std::tr1::shared_ptr < Vec3dParameter > vec3dParam)
  {
    m_vec3ds.push_back(vec3dParam);
  }

  void ParameterCollection::AddVec3fParameter(std::tr1::shared_ptr < Vec3fParameter > vec3fParam)
  {
    m_vec3fs.push_back(vec3fParam);
  }

  void ParameterCollection::AddVec3iParameter(std::tr1::shared_ptr < Vec3iParameter > vec3iParam)
  {
    m_vec3is.push_back(vec3iParam);
  }

  void ParameterCollection::AddVec2dParameter(std::tr1::shared_ptr < Vec2dParameter > vec2dParam)
  {
    m_vec2ds.push_back(vec2dParam);
  }

  void ParameterCollection::AddVec2fParameter(std::tr1::shared_ptr < Vec2fParameter > vec2fParam)
  {
    m_vec2fs.push_back(vec2fParam);
  }

  void ParameterCollection::AddVec2iParameter(std::tr1::shared_ptr < Vec2iParameter > vec2iParam)
  {
    m_vec2is.push_back(vec2iParam);
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

  std::vector < std::tr1::shared_ptr < FileParameter > > ParameterCollection::GetOpenFileParameters()
  {
    return m_ofiles;
  }

  std::vector < std::tr1::shared_ptr < FileParameter > > ParameterCollection::GetSaveFileParameters()
  {
    return m_sfiles;
  }

  std::vector < std::tr1::shared_ptr < BoolParameter > > ParameterCollection::GetBoolParameters()
  {
    return m_bools;
  }

  std::vector < std::tr1::shared_ptr < EnumParameter > > ParameterCollection::GetEnumParameters()
  {
    return m_enums;
  }

  std::vector < std::tr1::shared_ptr < Vec3dParameter > > ParameterCollection::GetVec3dParameters()
  {
    return m_vec3ds;
  }

  std::vector < std::tr1::shared_ptr < Vec3fParameter > > ParameterCollection::GetVec3fParameters()
  {
    return m_vec3fs;
  }

  std::vector < std::tr1::shared_ptr < Vec3iParameter > > ParameterCollection::GetVec3iParameters()
  {
    return m_vec3is;
  }

  std::vector < std::tr1::shared_ptr < Vec2dParameter > > ParameterCollection::GetVec2dParameters()
  {
    return m_vec2ds;
  }

  std::vector < std::tr1::shared_ptr < Vec2fParameter > > ParameterCollection::GetVec2fParameters()
  {
    return m_vec2fs;
  }

  std::vector < std::tr1::shared_ptr < Vec2iParameter > > ParameterCollection::GetVec2iParameters()
  {
    return m_vec2is;
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