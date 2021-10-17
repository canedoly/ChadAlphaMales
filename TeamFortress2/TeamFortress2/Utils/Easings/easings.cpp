#include "easing.h"
#include <time.h>
#include <cmath>

Animate gAnimate;

float Animate::easeOut(float start, float end, float speed)
{
	if (start > end || speed <= 1)
		return end;

	if (timer())
	{
		if (start < end)
			return start * speed;
		return end;
	}
	return start;
}

float Animate::easeIn(float start, float end, float speed)
{
	if (start < end || speed >= 1)
		return end;

	if (timer())
	{
		if (start > end)
			return start * speed;
		return end;
	}
	return start;
}

float Animate::linear(float start, float end, float speed)
{
	if (start < end)
		return start + speed;
	return end;
}

bool Animate::timer() // This is to make sure that the animations don't get calculated in a split-second
{
	int t = clock();
	static int i = 0;

	if (t > i)
	{
		i += 1;
		return true;
	}
	return false;
}