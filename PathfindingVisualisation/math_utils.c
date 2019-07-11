#include "math_utils.h"

// maps the value of an int from one range to another range
int imap(int val, int in_min, int in_max, int out_min, int out_max)
{
	return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}