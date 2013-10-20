using System.Collections.Generic;
using System.Linq;
using System.Windows.Media.Media3D;

namespace Jubjubnest.FlightControl.MathHelpers
{
	class ShiftingDoubleAverage
	{
		private Queue<double> samples = new Queue<double>();
		private int sampleCount;

		public ShiftingDoubleAverage(int samples)
		{
			sampleCount = samples;
		}

		public double Push(double sample)
		{
			samples.Enqueue(sample);
			if (samples.Count > sampleCount)
				samples.Dequeue();

			return samples.Average();
		}
	}
}
