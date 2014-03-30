#pragma once

class DeviceNotification
{
public:
	DeviceNotification(bool useNotifyValue);
	DeviceNotification(bool useNotifyValue, int NotifyValue);
	~DeviceNotification(void);

	void SetNotification(bool useNotifyValue, int NotifyValue=0);

	bool HasNotifyValue(void) { return m_useNotifyValue; }
	int GetNotifyValue(void) { return m_NotifyValue; }
	
private:
	bool m_useNotifyValue;
	int m_NotifyValue;
};
