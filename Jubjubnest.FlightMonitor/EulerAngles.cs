using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Jubjubnest.FlightControl
{
	class EulerAngles : ViewModelBase
	{
		public double Yaw
		{
			get { return _yaw; }
			set
			{
				_yaw = value;
				RaisePropertyChanged(() => Yaw);
			}
		}
		private double _yaw;

		public double Pitch
		{
			get { return _pitch; }
			set
			{
				_pitch = value;
				RaisePropertyChanged(() => Pitch);
			}
		}
		private double _pitch;

		public double Roll
		{
			get { return _roll; }
			set
			{
				_roll = value;
				RaisePropertyChanged(() => Roll);
			}
		}
		private double _roll;
	}
}
