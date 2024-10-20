import React, { useEffect, useState } from 'react';
import { View, Button, Text } from 'react-native';
import { BleManager } from 'react-native-ble-plx';
import { Buffer } from 'buffer';

const bleManager = new BleManager();

const SERVICE_UUID = 'cb4d1429-62b4-4e83-940e-77499e409770';  // Your service UUID
const CHARACTERISTIC_UUID = '119177b1-3c4d-4256-afd3-9f06ff01e1c6';  // Your characteristic UUID

const BLEConnection = () => {
  const [device, setDevice] = useState(null);
  const [isConnected, setIsConnected] = useState(false);

  useEffect(() => {
    return () => {
      // Cleanup: destroy the BLE manager instance on component unmount
      bleManager.destroy();
    };
  }, []);

  // Function to start scanning and connect to the ESP32
  const scanAndConnect = () => {
    bleManager.startDeviceScan(null, null, (error, scannedDevice) => {
      if (error) {
        console.error("Scan error: ", error);
        return;
      }

      // Look for your specific device name (replace "ESP32" with your actual device name if necessary)
      if (scannedDevice.name === 'ESP32-wroom') {
        bleManager.stopDeviceScan();
        connectToDevice(scannedDevice);
      }
    });
  };

  // Function to connect to the ESP32
  const connectToDevice = async (device) => {
    try {
      const connectedDevice = await device.connect();
      const services = await connectedDevice.discoverAllServicesAndCharacteristics();
      setDevice(connectedDevice);
      setIsConnected(true);
      console.log("Connected to ESP32!");
    } catch (error) {
      console.error("Connection error: ", error);
    }
  };

  // Function to send data to ESP32
  const sendDataToESP32 = async (data) => {
    if (device && isConnected) {
      const dataToSend = Buffer.from(data);  // Convert string to buffer
      try {
        await device.writeCharacteristicWithResponseForService(
          SERVICE_UUID,
          CHARACTERISTIC_UUID,
          dataToSend.toString('base64')  // BLE requires base64 encoding
        );
        console.log("Data sent: ", data);
      } catch (error) {
        console.error("Error sending data: ", error);
      }
    } else {
      console.warn("Not connected to ESP32");
    }
  };

  return (
    <View style={{ padding: 20 }}>
      <Button title="Scan & Connect to ESP32" onPress={scanAndConnect} />
      <Button
        title="Send Data to ESP32"
        onPress={() => sendDataToESP32("Hello ESP32!")}
        disabled={!isConnected}
      />
      <Text>{isConnected ? "Connected to ESP32" : "Not connected"}</Text>
    </View>
  );
};

export default BLEConnection;