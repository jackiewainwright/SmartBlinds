import React, { Component } from 'react';
import styled from 'styled-components/native';
import { Text, StyleSheet, Dimensions, TouchableOpacity } from 'react-native';
import Time from './Time';
import DateSelector from './DateSelector';
import { Subject } from 'rxjs';
import { debounceTime } from 'rxjs/operators';
import DeleteButton from './DeleteButton';
import { inject } from 'mobx-react';

const screenWidth = Math.round(Dimensions.get('window').width);

const CardContainer = styled.View`
  width: ${screenWidth - 20}px; /* 10px padding on each side */
  padding: 12px;
  background-color: #fffdfc;
  box-shadow: 0px 2px 3px rgba(0, 0, 0, 0.2);
  elevation: 6;
`;

const TimesContainer = styled.View`
  display: flex;
  flex-direction: row;
  width: 100%;
`;

const DateSelectorConatiner = styled.View`
  width: 100%;
  margin-top: 40px;
`;

const HorizontalDivider = styled.View`
  width: 100%;
  border-bottom-width: 1px;
  border-bottom-color: #d8d8d8;
  margin-top: 18px;
  margin-bottom: 9px;
`;

@inject('scheduleStore')
class ScheduleCard extends Component {
  days$ = new Subject();

  constructor(props) {
    super(props);
    this.state = {
      days: props.days
    }
  }

  componentDidMount() {
    this.days$
    .pipe(
      debounceTime(1000)
    )
    .subscribe(this.handleDaysChange);
  }

  handleDaysChange = async days => {
    this.props.scheduleStore.updateScheduleDays(this.props.scheduleId, days);
  };

  handleDelete = async () => {
    this.props.scheduleStore.deleteSchedule(this.props.scheduleId);
  };

  render() {
    return (
      <CardContainer>
        <TimesContainer>
          <Time time={this.props.times.from} action="DOWN" />
          <Time time={this.props.times.to} action="UP" />
        </TimesContainer>
        <DateSelectorConatiner>
          <DateSelector
            days={this.state.days}
            onChange={days => this.days$.next(days)}
          />
        </DateSelectorConatiner>

        <HorizontalDivider />

        <DeleteButton onPress={this.handleDelete} />
      </CardContainer>
    );
  }
}

export default ScheduleCard;
