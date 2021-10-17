#pragma once

class Animate
{
public:
	float easeOut(float start, float end, float speed);
	float easeIn(float start, float end, float speed);
	float linear(float start, float end, float speed);
private:
	bool timer();
};

extern Animate gAnimate;