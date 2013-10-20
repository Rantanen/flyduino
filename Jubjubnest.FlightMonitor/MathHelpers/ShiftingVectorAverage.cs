using System.Collections.Generic;
using System.Linq;
using System.Windows.Media.Media3D;

namespace Jubjubnest.FlightControl.MathHelpers
{
	class ShiftingVectorAverage
	{
		private Queue<Vector3D> samples = new Queue<Vector3D>();
		private int sampleCount;

		public ShiftingVectorAverage(int samples)
		{
			sampleCount = samples;
		}

		public Vector3D Push(Vector3D sample)
		{
			samples.Enqueue(sample);
			if (samples.Count > sampleCount)
				samples.Dequeue();

			var sum = samples.Aggregate((v1, v2) => v1 + v2);
			return sum / samples.Count;
		}
	}
}
