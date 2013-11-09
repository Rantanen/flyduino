using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Jubjubnest.FlightControl
{
	class ControlViewModel : ViewModelBase
	{
		public ControlViewModel(FlightControlConnection connection)
		{
			this.connection = connection;
		}

		private FlightControlConnection connection;

		private byte _power;
		public byte Power
		{
			get { return _power; }
			set
			{
				_power = value;
				RaisePropertyChanged(() => Power);
				connection.Send(new[] { value });
			}
		}
	}
}
