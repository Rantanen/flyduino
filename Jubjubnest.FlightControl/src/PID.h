
#ifndef _PID_H_
#define _PID_H_

class PID
{
	public:

		PID( float kp, float ki, float kd )
			: Kp( kp ), Ki( ki ), Kd( kd ), Pe( 0 ), Ie( 0 ), De( 0 )
		{
		}

		float Kp;
		float Ki;
		float Kd;
		float Pe;
		float Ie;
		float De;
		unsigned long lastUpdate;

		void update( float value, unsigned long timestamp )
		{
			if( lastUpdate == 0 )
			{
				lastUpdate = timestamp;
				return;
			}

			// Microsecond to second
			float delta = (timestamp - lastUpdate) / 1000000.0f;

			Pe = Kp * value;
			Ie += Ki * value * delta;
			De = Kd * value / delta;
		}

		void resetError()
		{
			Ie = 0;
		}

		float getValue()
		{
			return Pe + Ie + De;
		}
};

#endif
