using System;
using System.Linq;
using System.Collections.Generic;

namespace Jubjubnest.FlightControl.MathHelpers
{
	class FrequencyCounter
	{
		private Queue<DateTime> times = new Queue<DateTime>();

		public int Tick()
		{
			times.Enqueue(DateTime.Now);

			DateTime treshold = DateTime.Now - new TimeSpan(0, 0, 1);

			while (times.Peek() < treshold)
				times.Dequeue();

			return times.Count;
		}
	}
}
