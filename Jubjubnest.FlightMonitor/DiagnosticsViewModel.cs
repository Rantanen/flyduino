using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace Jubjubnest.FlightControl
{
	class DiagnosticsViewModel : ViewModelBase
	{
		public DiagnosticsViewModel()
		{

			// Assign a message handler for the log input
			connection.MessageReceived += (o, args) =>
				{
					var combined = DateTime.Now.ToString() + ": " + args.Message + Log;
					if (combined.Length > 1024*10)
						combined = combined.Substring(0, 1024*10);
					Log = combined;
				};

			// Populate port names
			this._portNames = SerialPort.GetPortNames();
			SendText = new RelayCommand(() => connection.Send(this.InputText));
			MotionModel = new MotionViewModel(connection);
		}

		private FlightControlConnection connection = new FlightControlConnection();
		public MotionViewModel MotionModel { get; protected set; }

		/// <summary>
		/// Gets or sets the Log text
		/// </summary>
		public string Log
		{
			get { return _log; }
			set
			{
				_log = value;
				RaisePropertyChanged(() => Log);
			}
		}
		private string _log;

		/// <summary>
		/// Gets the available serial ports
		/// </summary>
		public string[] PortNames
		{
			get { return _portNames; }
		}
		private readonly string[] _portNames;

		/// <summary>
		/// Gets or sets the selected port
		/// </summary>
		private string _selectedPort;
		public string SelectedPort
		{
			get { return _selectedPort; }
			set
			{
				_selectedPort = value;
				RaisePropertyChanged(() => SelectedPort);
				connection.Connect(value);
			}
		}

		/// <summary>
		/// Gets or sets the input text to be sent on Send-command
		/// </summary>
		private string _inputText;
		public string InputText
		{
			get { return _inputText; }
			set
			{
				_inputText = value;
				RaisePropertyChanged(() => InputText);
			}
		}

		/// <summary>
		/// Gets the command for sending the InputText over the serial port
		/// </summary>
		public RelayCommand SendText { get; protected set; }
	}
}
