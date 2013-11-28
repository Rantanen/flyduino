
#ifndef _PID_H_
#define _PID_H_

class PID
{
	public:

		PID( float kp, float ki, float kd, float limit )
			: Kp( kp ), Ki( ki ), Kd( kd ),
				limit( limit ),
				Pe( 0 ), Ie( 0 ), De( 0 )
		{
		}

		float Kp;
		float Ki;
		float Kd;
		float limit;
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
			De = Kd * value / delta;

			if( Pe + De < 50 )
			{
				Ie = constrain( Ie + Ki * value * delta, -50, 50 );
			}
			else
			{
				Ie *= 0.99;
			}
		}

		void resetError()
		{
			lastUpdate = micros();
			Ie = 0;
		}

		float getValue()
		{
			return constrain( Pe + Ie + De, -limit, limit );
		}
};

#endif
