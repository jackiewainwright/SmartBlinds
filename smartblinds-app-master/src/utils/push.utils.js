import { Permissions, Notifications, Constants } from 'expo';
import axios from 'axios';
import config from '../config/config';

async function requestPushPermissions() {
  const { status: existingStatus } = await Permissions.getAsync(Permissions.NOTIFICATIONS);
  let finalStatus = existingStatus;
  
  if (existingStatus !== 'granted') {
    const { status } = await Permissions.askAsync(Permissions.NOTIFICATIONS);
    finalStatus = status;
  }
  
  if (finalStatus !== 'granted') {
    global.alert('It is recommended to grant SmartBlinds permissions to send Push Notifications');
    return;
  }
  
  try {
    let token = await Notifications.getExpoPushTokenAsync();
    return token; 
  } catch (error) {
    alert(`Cannot retrieve push token - ${error}`);    
  }
}

async function registerForPush(token) {
  try {
    const result = await axios.post(`http://${config.server.host}/device/register`, {
      deviceId: Constants.installationId,
      token,
      prod: __DEV__
    });

    if (result.status === 500) {
      global.alert('Error: Cannot register device for push notifications');
      console.error(result.data);
      return;
    }

    console.info('Successfully registered for push notifications');
  } catch (error) {
    alert(`Cannot register for push notifications - ${error}`);    
  }
}

export {
  requestPushPermissions,
  registerForPush
};
