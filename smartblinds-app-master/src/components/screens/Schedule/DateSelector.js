import React, { Component, useState } from 'react'
import { Text, View } from 'react-native'
import styled from 'styled-components';
import Day from './Day';
import { anyDaySelected } from '../../../utils/time.utils';

const DaysWrapper = styled.View`
  flex-wrap: wrap;
  align-items: flex-start;
  flex-direction: row;
`;

const defaultDays = {
  mon: false,
  tue: false,
  wed: false,
  thu: false,
  fri: false,
  sat: false,
  sun: false
};

export default class DateSelector extends Component {
  constructor(props) {
    super(props);
    this.state = { days: props.days || defaultDays };
  }

  renderDays = () => {
    if (!this.state.days) {
      return null;
    }

    return Object.entries(this.state.days).map(([day, selected]) => {
      const handleDayPress = () => {
        const daysCopy = JSON.parse(JSON.stringify(this.state.days));
        daysCopy[day] = !daysCopy[day];

        if (!anyDaySelected(daysCopy)) {
          alert('You must select at least one day!');
          return;
        }

        this.setState({ days: daysCopy }, () => {
          this.props.onChange(this.state.days);
        });
      };

      return (
        <Day
          key={day}
          name={day}
          selected={selected}
          onPress={handleDayPress}
        />
      );
    });
  };

  render() {
    return (
      <DaysWrapper>
        {this.renderDays()}
      </DaysWrapper>
    )
  }
}
