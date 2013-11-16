
#ifndef _IFLIGHTSTATE_H_
#define _IFLIGHTSTATE_H_

class IFlightState
{
	public:
		virtual void update() = 0;
		virtual void start() = 0;
		virtual void stop() = 0;
};

#endif

