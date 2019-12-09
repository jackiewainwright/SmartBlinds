import React, { Component } from 'react';
import { Text } from 'react-native';
import styled from 'styled-components';
import { inject } from 'mobx-react';
import config from '../../../config/config';

import ViewWrapper from '../../layout/ViewWrapper';
import TimeRange from './TimeRange';
import FabButton from '../Schedule/FabButton';
import DateSelector from '../Schedule/DateSelector';
import { anyDaySelected } from '../../../utils/time.utils';

const { theme } = config;

const bottomPosition = '5%';
const sidePosition = '20px';

const CancelButtonContainer = styled.View`
  position: absolute;
  bottom: ${bottomPosition};
  left: ${sidePosition};
`;

const ScheduleButtonContainer = styled.View`
  position: absolute;
  bottom: ${bottomPosition};
  right: ${sidePosition};
`;

const DateSelectorContainer = styled.View`
  position: absolute;
  bottom: 16%;
  align-items: center;
  justify-content: center;
  width: 100%;
`;

@inject('scheduleStore')
export default class componentName extends Component {
  constructor(props) {
    super(props);
    this.state = {
      start: { hour: 20, minute: 0 },
      end: { hour: 1, minute: 0 },
      days: null
    }
  }

  handleTimeRangeUpdate = ({ start, end }) => {
    this.setState({
      start: {
        hour: start.h,
        minute: start.m
      },
      end: {
        hour: end.h,
        minute: end.m
      }
    });
  };

  handleDaysChange = days => {
    this.setState({ days });
  };

  handleSubmitSchedule = async () => {
    const { start, end, days } = this.state;
    await this.props.scheduleStore.setSchedule({ start, end, days });
    this.props.navigation.navigate('ScheduleStack');
  };

  render() {
    const { navigate } = this.props.navigation;
    const disabled = !this.state.days || !anyDaySelected(this.state.days);

    return (
      <ViewWrapper>
        <TimeRange onUpdate={this.handleTimeRangeUpdate} />

        <DateSelectorContainer>
          <DateSelector onChange={this.handleDaysChange} />
        </DateSelectorContainer>

        <CancelButtonContainer>
          <FabButton color="#abb2bc" icon="ios-close" onPress={() => navigate('ScheduleStack')} />
        </CancelButtonContainer>

        <ScheduleButtonContainer>
          <FabButton
            color={disabled ? '#abb2bc' : theme.color.primary}
            icon="ios-timer"
            disabled={disabled}
            onPress={() => disabled ? null : this.handleSubmitSchedule()} />
        </ScheduleButtonContainer>
      </ViewWrapper>
    );
  }
}
