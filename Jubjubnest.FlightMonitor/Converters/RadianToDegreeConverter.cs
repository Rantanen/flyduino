using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace Jubjubnest.FlightControl.Converters
{
	public class RadianToDegreeConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			var n = value as double?;
			if (n.HasValue)
				return n.Value*360/(2*Math.PI);
			return 0;
		}

		public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			var n = value as int?;
			if (n.HasValue)
				return n.Value*2*Math.PI/360;
			return 0;
		}
	}
}
