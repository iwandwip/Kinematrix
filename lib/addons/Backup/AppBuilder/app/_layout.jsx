import React, { useEffect } from 'react';
import { Stack } from 'expo-router';
import { StatusBar } from 'expo-status-bar';
import { SafeAreaProvider } from 'react-native-safe-area-context';

export default function RootLayout() {
  useEffect(() => {
    console.log('App initialized');
    
    return () => {
      console.log('App cleanup');
    };
  }, []);

  return (
    <SafeAreaProvider>
        <StatusBar style="dark" backgroundColor="transparent" translucent={false} />
        <Stack screenOptions={{ headerShown: false }}>
          <Stack.Screen name="index" />
        </Stack>
    </SafeAreaProvider>
  );
}