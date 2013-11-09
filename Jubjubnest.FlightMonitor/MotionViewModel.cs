using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Media3D;
using Jubjubnest.FlightControl.MathHelpers;

namespace Jubjubnest.FlightControl
{
	class MotionViewModel : ViewModelBase
	{
		public MotionViewModel(FlightControlConnection connection)
		{
			// Register handler for the orientation update
			connection.RegisterHandler("ROT", rot =>
				{
					UpdateFrequency = updateFrequencyCounter.Tick();
					Rotation = new Quaternion(
						double.Parse(rot[0], CultureInfo.InvariantCulture),
						double.Parse(rot[1], CultureInfo.InvariantCulture),
						double.Parse(rot[2], CultureInfo.InvariantCulture),
						double.Parse(rot[3], CultureInfo.InvariantCulture));
					;
				});

			// Register handler for the heading update
			connection.RegisterHandler("HEAD", head =>
				{
					Heading = new Quaternion(
						double.Parse(head[0], CultureInfo.InvariantCulture),
						double.Parse(head[1], CultureInfo.InvariantCulture),
						double.Parse(head[2], CultureInfo.InvariantCulture),
						double.Parse(head[3], CultureInfo.InvariantCulture));
				});

			connection.RegisterHandler("OFFSET", offset =>
				{
					AngularOffset.Yaw = double.Parse(offset[0], CultureInfo.InvariantCulture);
					AngularOffset.Pitch = double.Parse(offset[1], CultureInfo.InvariantCulture);
					AngularOffset.Roll = double.Parse(offset[2], CultureInfo.InvariantCulture);
				});

			_engines = new List<EngineViewModel>
				{
					new EngineViewModel(5, 10, 10),
					new EngineViewModel(6, 10, -10),
					new EngineViewModel(9, -10, -10),
					new EngineViewModel(10, -10, 10)
				};

			var enginesByPin = _engines.ToDictionary(e => e.Pin);
			connection.RegisterHandler("ENG", eng =>
				{
					var pin = int.Parse(eng[0]);
					enginesByPin[pin].Power = int.Parse(eng[1]);
				});

			ResetCamera = new RelayCommand(() => CameraRotation = Rotation); // * new Quaternion(0, 0, 1, 0));
			AngularOffset = new EulerAngles();
		}

		private readonly FrequencyCounter updateFrequencyCounter = new FrequencyCounter();
		private readonly ShiftingVectorAverage headingAverage = new ShiftingVectorAverage( 1 );

		/// <summary>
		/// Gets the current update frequency for the orientation information
		/// </summary>
		public int UpdateFrequency
		{
			get { return _updateFrequency; }
			protected set
			{
				_updateFrequency = value;
				RaisePropertyChanged(() => UpdateFrequency);
			}
		}
		private int _updateFrequency;

		/// <summary>
		/// Gets the current heading vector
		/// </summary>
		public Quaternion Heading
		{
			get { return _heading; }
			set
			{
				_heading = value;
				RaisePropertyChanged(() => Heading);
			}
		}
		private Quaternion _heading;

		/// <summary>
		/// Gets the current orientation
		/// </summary>
		public Quaternion Rotation
		{
			get { return _rotation; }
			set
			{
				_rotation = value;
				RaisePropertyChanged(() => Rotation);
			}
		}
		private Quaternion _rotation;

		public EulerAngles AngularOffset { get; protected set; }

		/// <summary>
		/// Gets the current camera rotation
		/// </summary>
		public Quaternion CameraRotation
		{
			get { return _cameraRotation; }
			protected set
			{
				_cameraRotation = value;
				RaisePropertyChanged(() => CameraRotation);
			}
		}
		private Quaternion _cameraRotation;

		public IEnumerable<EngineViewModel> Engines
		{
			get { return _engines; }
		}
		private List<EngineViewModel> _engines;


		public RelayCommand ResetCamera { get; protected set; }
	}
}
