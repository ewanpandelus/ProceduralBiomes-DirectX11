#pragma once

//this struct is the information we want to pass thru to the game processing.  We never want to be dealing with the hardware directly.
//thats the job for this class.  The other benefit of this abstraction of input data is that when we want to change the input
//to different hardware.  its really easily done in this class by mapping to the inputcommands. 

struct InputCommands
{
	bool forward;
	bool back;
	bool right;
	bool left;
	bool rotRight;
	bool rotLeft;
	bool generate;
	bool rotY = false;
	bool rotX = false;
};


class Input
{
public:
	Input();
	~Input();
	void Initialise(HWND window);
	void Update();
	bool Quit();
	InputCommands getGameInput();
	float GetDeltaX();
	float GetDeltaY();


private:
	bool m_quitApp;
	std::unique_ptr<DirectX::Keyboard>	m_keyboard;
	std::unique_ptr<DirectX::Mouse>		m_mouse;
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;
	DirectX::Mouse::ButtonStateTracker		m_MouseTracker;
	InputCommands							m_GameInput;
	float prevX = 0;
	float prevY = 0;
	float deltaX = 0;
	float deltaY = 0;
};

