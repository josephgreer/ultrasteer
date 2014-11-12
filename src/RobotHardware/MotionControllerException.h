#include <stdexcept>
#include <string>

class MotionControllerException : public std::runtime_error 
{
public:
    MotionControllerException(const std::string& message) : std::runtime_error(message) 
	{
		m_message = message;
	};

	void AddInfo(const std::string& message)
	{
		//add additional information to the exception after a delimiter
		m_message.append(" - ");
		m_message.append(message);
	};

	const char* what() const throw() 
	{ 
		return m_message.c_str(); 
	};

private:
	std::string m_message;
};