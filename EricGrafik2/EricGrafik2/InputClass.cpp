#include "InputClass.h"



InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass &)
{
}


InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	int i;
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
}

void InputClass::KeyDown(unsigned int p_input)
{
	m_keys[p_input] = true;
}

void InputClass::KeyUp(unsigned int p_input)
{
	m_keys[p_input] = false;
}

bool InputClass::IsKeyDown(unsigned int p_key)
{
	return m_keys[p_key];
}

