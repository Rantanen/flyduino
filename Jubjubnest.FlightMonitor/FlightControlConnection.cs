﻿using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace Jubjubnest.FlightControl
{
	internal class FlightControlConnection
	{
		private Thread readingThread;
		private SerialPort serialPort;
		private object threadGuard = new object();

		[MethodImpl(MethodImplOptions.Synchronized)]
		public void Connect(string port)
		{
			// Disconnect the old connection in case.
			Disconnect();

			if (string.IsNullOrEmpty(port)) return;

			serialPort = new SerialPort(port, 115200, Parity.None, 8, StopBits.One);

			readingThread = new Thread(Read);
			readingThread.IsBackground = true;
			readingThread.Start();
		}

		[MethodImpl(MethodImplOptions.Synchronized)]
		public void Disconnect()
		{
			if (serialPort == null)
				return;

			lock (threadGuard)
			{
				serialPort.Close();
				serialPort = null;
			}

			readingThread.Join();
		}

		private void Read()
		{
			serialPort.ReadTimeout = 100;
			lock (threadGuard)
			{
				serialPort.Open();

				while (serialPort != null)
				{
					try
					{
						while (serialPort.BytesToRead > 0)
						{
							var msg = serialPort.ReadLine();
							Dispatcher.CurrentDispatcher.Invoke(() => RaiseMessageReceived(msg));
						}
					}
					catch (TimeoutException)
					{
					}

					Monitor.Wait(threadGuard, 100);
				}
			}
		}

		public void Send(string text)
		{
			lock (threadGuard)
			{
				serialPort.Write(text + "\n");
			}
		}

		public void Send(byte[] b)
		{
			lock (threadGuard)
			{
				serialPort.Write(b, 0, b.Length);
				serialPort.Write(new[] { (byte)0 }, 0, 1);
			}
		}

		private void RaiseMessageReceived(string msg)
		{
			var e = MessageReceived;
			if (e == null) return;


			e(this, new FlightControlMessageEventArgs( msg ));
		}

		public void RegisterHandler(string type, Action<string[]> handler)
		{
			this.MessageReceived += (e, args) =>
				{
					try
					{
						if (args.MessageType == type)
						{
							handler(args.MessageArgs);
						}
					}
					catch { }
				};
		}

		public event EventHandler<FlightControlMessageEventArgs> MessageReceived;

		public class FlightControlMessageEventArgs : EventArgs
		{
			public FlightControlMessageEventArgs(string msg)
			{
				Message = msg;

				var split = msg.Split('\t');
				MessageType = split[0];
				MessageArgs = split.Skip(1).ToArray();
			}

			public string Message { get; protected set; }
			public string MessageType { get; protected set; }
			public string[] MessageArgs { get; protected set; }
		}
	}
}
