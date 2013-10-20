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
					Heading = headingAverage.Push(new Vector3D(
						double.Parse(head[0], CultureInfo.InvariantCulture)*45,
						double.Parse(head[1], CultureInfo.InvariantCulture)*-45,
						double.Parse(head[2], CultureInfo.InvariantCulture)*45));
				});

			ResetCamera = new RelayCommand(() => CameraRotation = Rotation * new Quaternion(0, 0, 1, 0));
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
		public Vector3D Heading
		{
			get { return _heading; }
			set
			{
				_heading = value;
				RaisePropertyChanged(() => Heading);
			}
		}
		private Vector3D _heading;

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

		/// <summary>
		/// Gets the front left engine power
		/// </summary>
		public float FrontLeftEnginePower
		{
			get { return _frontLeftEnginePower; }
			protected set
			{
				_frontLeftEnginePower = value;
				RaisePropertyChanged(() => FrontLeftEnginePower);
			}
		}
		private float _frontLeftEnginePower;

		/// <summary>
		/// Gets the front right engine power
		/// </summary>
		public float FrontRightEnginePower
		{
			get { return _frontRightEnginePower; }
			protected set
			{
				_frontRightEnginePower = value;
				RaisePropertyChanged(() => FrontRightEnginePower);
			}
		}
		private float _frontRightEnginePower;

		/// <summary>
		/// Gets the rear left engine power
		/// </summary>
		public float RearLeftEnginePower
		{
			get { return _rearLeftEnginePower; }
			protected set
			{
				_rearLeftEnginePower = value;
				RaisePropertyChanged(() => RearLeftEnginePower);
			}
		}
		private float _rearLeftEnginePower;

		/// <summary>
		/// Gets the rear right engine power
		/// </summary>
		public float RearRightEnginePower
		{
			get { return _rearRightEnginePower; }
			protected set
			{
				_rearRightEnginePower = value;
				RaisePropertyChanged(() => RearRightEnginePower);
			}
		}
		private float _rearRightEnginePower;

		public RelayCommand ResetCamera { get; protected set; }
	}
}
