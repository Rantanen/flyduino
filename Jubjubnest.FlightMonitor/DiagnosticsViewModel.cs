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
		private static Dictionary<string, ErrorLevel> msgLevels = new Dictionary<string, ErrorLevel>
			{
				{"DEBUG", ErrorLevel.Debug},
				{"INFO", ErrorLevel.Info},
				{"WARN", ErrorLevel.Warning},
				{"ERROR", ErrorLevel.Error},
				{"FATAL", ErrorLevel.Fatal}
			};
		public DiagnosticsViewModel()
		{

			// Assign a message handler for the log input
			connection.MessageReceived += (o, args) =>
				{
					if (this.TraceLevel == ErrorLevel.None) return;

					if (args.MessageType != "TRACE")
					{
						if (this.TraceLevel != ErrorLevel.Diagnostic)
							return;
					}
					else
					{
						// Strip the escape code
						string header = args.MessageArgs[0].Substring("x[xxm".Length);

						ErrorLevel errorLevel = msgLevels[ header ];
						if (errorLevel < TraceLevel) return;
					}

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

		public enum ErrorLevel { Diagnostic, Debug, Info, Warning, Error, Fatal, None }

		/// <summary>
		/// Gets the available trace levels
		/// </summary>
		public Dictionary<string, ErrorLevel> AvailableLevels
		{
			get
			{
				return Enum.GetValues(typeof (ErrorLevel))
					.Cast<ErrorLevel>()
					.ToDictionary( el => Enum.GetName(typeof (ErrorLevel), el));
			}
		}

		/// <summary>
		/// Gets or sets the current trace level shown in the log
		/// </summary>
		public ErrorLevel TraceLevel
		{
			get { return _traceLevel; }
			set
			{
				_traceLevel = value;
				RaisePropertyChanged(() => TraceLevel);
			}
		}
		private ErrorLevel _traceLevel;

		/// <summary>
		/// Gets the command for sending the InputText over the serial port
		/// </summary>
		public RelayCommand SendText { get; protected set; }
	}
}
