import React from 'react';
import { Platform } from 'react-native';
import { createStackNavigator, createBottomTabNavigator } from 'react-navigation';

import TabBarIcon from '../TabBarIcon';
import ManualScreen from '../screens/Manual/ManualScreen';
import ScheduleScreen from '../screens/Schedule/ScheduleScreen';
import SensorScreen from '../screens/Sensor/SensorScreen';

const ManualStack = createStackNavigator({
  Manual: ManualScreen,
});

ManualStack.navigationOptions = {
  tabBarLabel: 'Manual',
  tabBarIcon: ({ focused }) => (
    <TabBarIcon
      focused={focused}
      name="ios-switch"
    />
  ),
};

const ScheduleStack = createStackNavigator({
  Schedule: ScheduleScreen,
});

ScheduleStack.navigationOptions = {
  tabBarLabel: 'Schedule',
  tabBarIcon: ({ focused }) => (
    <TabBarIcon
      focused={focused}
      name="ios-calendar"
    />
  ),
};

const SensorStack = createStackNavigator({
  Schedule: SensorScreen,
});

SensorStack.navigationOptions = {
  tabBarLabel: 'Sensor',
  tabBarIcon: ({ focused }) => (
    <TabBarIcon
      focused={focused}
      name="ios-aperture"
    />
  ),
};

export default createBottomTabNavigator({
  ManualStack,
  ScheduleStack,
  SensorStack,
});
