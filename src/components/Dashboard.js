import React, { useState, useEffect } from 'react';
import database from '../firebase';
import { ref, onValue } from 'firebase/database';
import MapComponent from './Map';
import LineChartComponent from './LineChart';
import DataTable from './DataTable';

const Dashboard = () => {
  const [device, setDevice] = useState('device1'); // Default to device1
  const [gps, setGps] = useState({ latitude: 0, longitude: 0 });
  const [temperatureData, setTemperatureData] = useState([]);
  const [pressureData, setPressureData] = useState([]);
  const [altitudeData, setAltitudeData] = useState([]);
  const [accelerationData, setAccelerationData] = useState([]);
  const [rotationData, setRotationData] = useState([]);

  useEffect(() => {
    const fetchData = () => {
      const sensorsRef = ref(database, `/devices/${device}/sensor_data`);
      onValue(sensorsRef, (snapshot) => {
        const data = snapshot.val();
        console.log("Firebase Data: ", data); // Log fetched data for debugging

        if (data) {
          try {
            const tempData = [];
            const presData = [];
            const altData = [];
            const accelData = [];
            const rotData = [];

            Object.keys(data).forEach(timestamp => {
              const sensorData = data[timestamp];

              // Update GPS coordinates with the last entry
              if (sensorData.gps) {
                setGps({ latitude: sensorData.gps.latitude || 0, longitude: sensorData.gps.longitude || 0 });
              }

              // Convert Unix timestamp to local time without adding offset
              const date = new Date(parseInt(timestamp) * 1000);
              const timeString = date.toLocaleString('en-IN', {
                hour: '2-digit',
                minute: '2-digit',
                second: '2-digit',
                day: '2-digit',
                month: '2-digit',
                year: 'numeric',
                hour12: true // Adjusts to 12-hour clock
              });

              // Extract temperature data
              tempData.push({
                time: timeString,
                temperature: sensorData.temperature || 0,
              });

              // Extract pressure data
              presData.push({
                time: timeString,
                pressure: sensorData.pressure || 0,
              });

              // Extract altitude data
              altData.push({
                time: timeString,
                altitude: sensorData.altitude || 0,
              });

              // Extract acceleration data
              accelData.push({
                time: timeString,
                x: sensorData.gyro?.acceleration?.x || 0,
                y: sensorData.gyro?.acceleration?.y || 0,
                z: sensorData.gyro?.acceleration?.z || 0,
              });

              // Extract rotation data
              rotData.push({
                time: timeString,
                x: sensorData.gyro?.rotation?.x || 0,
                y: sensorData.gyro?.rotation?.y || 0,
                z: sensorData.gyro?.rotation?.z || 0,
              });
            });

            setTemperatureData(tempData);
            setPressureData(presData);
            setAltitudeData(altData);
            setAccelerationData(accelData);
            setRotationData(rotData);

          } catch (error) {
            console.error("Error processing data: ", error);
          }
        } else {
          console.error("No data found in Firebase");
        }
      });
    };

    fetchData();
  }, [device]); // Refetch data when the selected device changes

  const handleDeviceChange = (e) => {
    setDevice(e.target.value);
  };

  const accelerationColumns = [
    { Header: 'Time', accessor: 'time' },
    { Header: 'X (m/s²)', accessor: 'x' },
    { Header: 'Y (m/s²)', accessor: 'y' },
    { Header: 'Z (m/s²)', accessor: 'z' },
  ];

  const rotationColumns = [
    { Header: 'Time', accessor: 'time' },
    { Header: 'X (rad/s)', accessor: 'x' },
    { Header: 'Y (rad/s)', accessor: 'y' },
    { Header: 'Z (rad/s)', accessor: 'z' },
  ];

  return (
    <div style={{ backgroundColor: '#333', color: '#fff', padding: '20px' }}>
      <h1 style={{ color: '#fff' }}>Sensor Dashboard</h1>

      {/* Dropdown for device selection */}
      <select onChange={handleDeviceChange} value={device} style={{ marginBottom: '20px', padding: '10px', fontSize: '16px' }}>
        <option value="device1">Device 1</option>
        <option value="device2">Device 2</option>
      </select>

      <MapComponent latitude={gps.latitude} longitude={gps.longitude} />
      
      <div style={{ display: 'flex', justifyContent: 'space-between', marginTop: '20px' }}>
        {/* Line charts for temperature, pressure, and altitude */}
        <LineChartComponent data={temperatureData} dataKey="temperature" lineColor="#8884d8" title="Temperature" width={300} height={300} />
        <LineChartComponent data={pressureData} dataKey="pressure" lineColor="#82ca9d" title="Pressure" width={300} height={300} />
        <LineChartComponent data={altitudeData} dataKey="altitude" lineColor="#ff7300" title="Altitude" width={300} height={300} />
      </div>

      {/* Acceleration Data Table */}
      <h3 style={{ color: '#40E0D0', marginTop: '20px' }}>Acceleration Data</h3>
      <DataTable columns={accelerationColumns} data={accelerationData} textStyle={{ color: '#fff' }} />

      {/* Rotation Data Table */}
      <h3 style={{ color: '#f06292', marginTop: '20px' }}>Rotation Data</h3>
      <DataTable columns={rotationColumns} data={rotationData} textStyle={{ color: '#fff' }} />
    </div>
  );
};

export default Dashboard;
