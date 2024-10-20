import React, { useState, useEffect } from 'react';
import { StyleSheet, View, TextInput, Button, Text, ScrollView, Alert } from 'react-native';
import BLEConnection from '@/components/BLEConnection';
import MapView, { Marker } from 'react-native-maps';
import MapViewDirections from 'react-native-maps-directions';
import * as Location from 'expo-location';
import axios from 'axios'; // Use Axios for sending data
import { ThemedView } from '@/components/ThemedView';
const origin = {latitude: 37.3318456, longitude: -122.0296002};
const destination = {latitude: 37.771707, longitude: -122.4053769};

const config = require('./config.js');


const App = () => {
  const [location, setLocation] = useState(null);
  const [destination, setDestination] = useState('');
  const [destinationCoords, setDestinationCoords] = useState(null);
  const [steps, setSteps] = useState([]); 
  const [currentStepIndex, setCurrentStepIndex] = useState(0);
  const [region, setRegion] = useState({
    latitude: 37.78825,
    longitude: -122.4324,
    latitudeDelta: 0.0922,
    longitudeDelta: 0.0421,
  });

  useEffect(() => {
    (async () => {
      let { status } = await Location.requestForegroundPermissionsAsync();
      if (status !== 'granted') {
        console.log('Permission to access location was denied');
        return;
      }
      let location = await Location.getCurrentPositionAsync({});
      setLocation(location.coords);
      setRegion({
        ...region,
        latitude: location.coords.latitude,
        longitude: location.coords.longitude,
      });
    })();
  }, []);

  const handleDestinationChange = async () => {
    try {
      const response = await fetch(
        `https://maps.googleapis.com/maps/api/geocode/json?address=${destination}&key=AIzaSyCcV-LGTK6oh-d4wGwrUIXnUqsOt0qT42M`
      );
      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
    }
      const data = await response.json();
      console.log(data);
      const location = data.results[0].geometry.location;
      setDestinationCoords({
        latitude: location.lat,
        longitude: location.lng,
      });
      console.log(region);

      const directionsResponse = await fetch(
        `https://maps.googleapis.com/maps/api/directions/json?origin=Disneyland&destination=Universal+Studios+Hollywood&key=AIzaSyCcV-LGTK6oh-d4wGwrUIXnUqsOt0qT42M`
      );
      if (!directionsResponse.ok) {
        throw new Error(`HTTP error! Status: ${directionsResponse.status}`);
    }
      const directionsData = await directionsResponse.json();
      
      const routeSteps = directionsData.routes[0].legs[0].steps;
      setSteps(routeSteps);
      setCurrentStepIndex(0); // Reset to the first step

      sendToDevice(routeSteps[0]); // Send the first step to the external device
    } catch (error) {
      console.log('Error fetching directions:', error);
    }
  };

  // Function to send the current step to an external device (simulated)
  const sendToDevice = async (step) => {
    try {
      // Simulate sending the step data (you can replace this with actual hardware logic)
      const stepData = {
        instructions: step.html_instructions.replace(/<[^>]+>/g, ''), // Remove HTML tags
        distance: step.distance.text,
      };

      // Simulate sending data to an external device or server
      await axios.post('https://your-server.com/device-endpoint', stepData);

      Alert.alert('Step Sent', `Sent: ${stepData.instructions}`);
    } catch (error) {
      console.error('Error sending to device:', error);
    }
  };

  useEffect(() => {
    const locationInterval = setInterval(async () => {
      if (steps.length > 0 && location) {
        const userLocation = await Location.getCurrentPositionAsync({});
        const userCoords = {
          latitude: userLocation.coords.latitude,
          longitude: userLocation.coords.longitude,
        };

        if (currentStepIndex < steps.length) {
          const nextStep = steps[currentStepIndex];
          const nextStepLocation = {
            latitude: nextStep.end_location.lat,
            longitude: nextStep.end_location.lng,
          };

          const distance = getDistance(userCoords, nextStepLocation);

          if (distance < 50) {
            const newIndex = currentStepIndex + 1;
            setCurrentStepIndex(newIndex);
            sendToDevice(steps[newIndex]); // Send the next step to the external device
          }
        }
      }
    }, 2000);

    return () => clearInterval(locationInterval);
  }, [location, steps, currentStepIndex]);

  const getDistance = (coords1, coords2) => {
    const toRad = (value) => (value * Math.PI) / 180;
    const R = 6371; 
    const dLat = toRad(coords2.latitude - coords1.latitude);
    const dLon = toRad(coords2.longitude - coords1.longitude);
    const a =
      Math.sin(dLat / 2) * Math.sin(dLat / 2) +
      Math.cos(toRad(coords1.latitude)) *
        Math.cos(toRad(coords2.latitude)) *
        Math.sin(dLon / 2) *
        Math.sin(dLon / 2);
    const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
    return R * c * 1000; // Distance in meters
  };

  return (
    <View style={styles.container}>
      {/* {location && (
        <MapView
          style={styles.map}
          region={region}
          showsUserLocation={true}
        >
        {destinationCoords && (
          <>
          <Marker coordinate={destinationCoords} />
            <MapViewDirections
              origin={{ latitude: location.latitude, longitude: location.longitude }}
              destination={destinationCoords}
              apikey={config.GOOGLE_MAPS_APIKEY}
              strokeWidth={4}
              strokeColor="blue"
            />
          </>
        )}
        </MapView>
      )} */}
      {/* <MapView
          style={styles.map}
          initialRegion={{
            latitude: 37.78825,
            longitude: -122.4324,
            latitudeDelta: 0.0922,
            longitudeDelta: 0.0421,
          }}
          showsUserLocation={true}
        ></MapView> */}
        
      <View style={styles.inputContainer}>
        <TextInput
          style={styles.input}
          placeholder="Enter destination"
          value={destination}
          onChangeText={setDestination}
        />
       <Button title="Get Directions" onPress={handleDestinationChange} />
      </View>
      {/* <BLEConnection /> */}
      {steps.length > 0 && currentStepIndex < steps.length && (
        <View style={styles.directionContainer}>
          <Text style={styles.directionText}>
            {steps[currentStepIndex].html_instructions.replace(/<[^>]+>/g, '')} - {steps[currentStepIndex].distance.text}
          </Text>
        </View>
      )}
    </View>
    // <ThemedView></ThemedView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  map: {
    flex: 1,
    height: 10,
    width: 10,
  },
  inputContainer: {
    position: 'absolute',
    top: 50,
    left: 10,
    right: 10,
    backgroundColor: 'white',
    padding: 10,
    borderRadius: 8,
  },
  input: {
    height: 40,
    borderColor: 'gray',
    borderWidth: 1,
    marginBottom: 10,
    paddingHorizontal: 10,
  },
  directionContainer: {
    position: 'absolute',
    bottom: 10,
    left: 10,
    right: 10,
    backgroundColor: 'white',
    padding: 10,
    borderRadius: 8,
  },
  directionText: {
    fontSize: 16,
  },
});

export default App;
