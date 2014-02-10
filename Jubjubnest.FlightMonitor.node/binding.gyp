{
  'targets': [
    {
      'target_name': 'node-flightcontrol',
      'sources': [
        'src/node-flightcontrol.cc',
      ],
      'include_dirs': [
        '../Jubjubnest.FlightControl/src',
        '../Jubjubnest.FlightControl/lib/MPU6050'
      ],
      'conditions': [
      ],
    },
  ],
}
