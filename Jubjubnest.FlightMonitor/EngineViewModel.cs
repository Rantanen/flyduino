using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Jubjubnest.FlightControl
{
	class EngineViewModel : ViewModelBase
	{
		public EngineViewModel(int pin, double offsetX, double offsetY)
		{
			Pin = pin;
			OffsetX = offsetX;
			OffsetY = offsetY;
		}

		public int Pin { get; protected set; }
		public double OffsetX { get; protected set; }
		public double OffsetY { get; protected set; }

		public int Power
		{
			get { return _power; }
			set
			{
				_power = value;
				RaisePropertyChanged(() => Power);
			}
		}

		private int _power;
	}
}
