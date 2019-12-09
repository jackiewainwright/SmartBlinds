import React from 'react';
import {
  StyleSheet,
  Text,
  View,
  ScrollView
} from 'react-native';
import { inject, observer } from 'mobx-react/native';
import styled from 'styled-components';
import ScheduleCard from './ScheduleCard';
import { Header } from '../../layout';
import FabButton from './FabButton';
import ViewWrapper from '../../layout/ViewWrapper';
import config from '../../../config/config';

const { theme } = config;

const AddButtonFloatingContainer = styled.View`
  position: absolute;
  bottom: 20px;
  right: 20px;
`;

@inject('scheduleStore')
@observer
export default class ScheduleScreen extends React.Component {
  static navigationOptions = {
    header: null,
  };

  componentDidMount() {
    this.props.navigation.addListener('willFocus', (route) => {
      console.log('Schedule screen focused, fetching schedules');
      this.fetchSchedules();
    });
  }

  fetchSchedules = () => {
    this.props.scheduleStore.fetchAllSchedules();
  };

  renderSchedules = () => {
    const schedules = this.props.scheduleStore.schedules;

    if (!schedules.length) {
      return (
        <Text>You don't have any schedules.</Text>
      );
    }

    return schedules.map(schedule => (
      <View style={{ marginBottom: 20 }} key={schedule.scheduleId}>
        <ScheduleCard
          scheduleId={schedule.scheduleId}
          days={schedule.days}
          times={{ from: schedule.from, to: schedule.to }}
        />
      </View>
    ));
  }

  render() {
    const { navigate } = this.props.navigation;

    return (
      <ViewWrapper>
        <Header />
        <ScrollView contentContainerStyle={{ alignItems: 'center' }}>
          {this.renderSchedules()}
        </ScrollView>
        <AddButtonFloatingContainer>
          <FabButton color={theme.color.primary} icon="ios-add" onPress={() => navigate('AddSchedule')} />
        </AddButtonFloatingContainer>
      </ViewWrapper>
    );
  }
}
