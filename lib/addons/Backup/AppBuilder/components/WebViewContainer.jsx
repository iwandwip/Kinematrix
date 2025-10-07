import React from 'react';
import { StyleSheet } from 'react-native';
import WebView from 'react-native-webview';

export default function WebViewContainer() {
  const htmlSource = {
    uri: 'file:///android_asset/web/index.html'
  };

  return (
    <WebView
      source={htmlSource}
      style={styles.webview}
      javaScriptEnabled={true}
      domStorageEnabled={true}
      allowFileAccess={true}
      allowUniversalAccessFromFileURLs={true}
      mixedContentMode="compatibility"
    />
  );
}

const styles = StyleSheet.create({
  webview: {
    flex: 1,
  },
});