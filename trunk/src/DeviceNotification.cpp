#include "DeviceNotification.h"

/// \brief		Parameterized Constructor
DeviceNotification::DeviceNotification(bool useNotifyValue)
{ 
	m_useNotifyValue = useNotifyValue;
}

/// \brief		Parameterized Constructor
DeviceNotification::DeviceNotification(bool useNotifyValue, int NotifyValue)
{
	SetNotification(useNotifyValue, NotifyValue);
}

/// \brief		Destructor
DeviceNotification::~DeviceNotification(void)
{
}

/// \brief		Set notification parameters
/// \details	If useTarget is set to false, atValue needs to be set.
void DeviceNotification::SetNotification(bool useNotifyValue, int NotifyValue/*=0*/)
{
	m_useNotifyValue = useNotifyValue;
	
	//Only set if we need to because it has been requested
	if(m_useNotifyValue)
	{
		m_NotifyValue = NotifyValue;
	}
}