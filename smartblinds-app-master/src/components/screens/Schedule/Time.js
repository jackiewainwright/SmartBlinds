import React, { Component } from 'react';
import { Text } from 'react-native';
import styled from 'styled-components';
import config from '../../../config/config';
import { convertUTCToLocal, getTimeDisplayString } from '../../../utils/time.utils';

const { theme } = config;

const TimeContainer = styled.View`
  display: flex;
  flex: 1;
  justify-content: center;
  width: 100%;
`;

const Action = styled.Text`
  color: ${theme.font.normal};
  font-size: 12px;
  margin-bottom: 10px;
`;

const TimeStamp = styled.Text`
  color: ${theme.font.normal};
  font-size: 20px;
`;

const TimeStampIndication = styled.Text`
  font-size: 10px;
`;

export default class Time extends Component {
  render() {
    const converted = convertUTCToLocal(this.props.time);
    const hour = converted.hour();
    const minute = converted.minute();

    const timeDisplayString = getTimeDisplayString({ h: hour, m: minute });

    return (
      <TimeContainer>
        <Action>
          {this.props.action}
        </Action>
        <TimeStamp>
          {timeDisplayString}
        </TimeStamp>
      </TimeContainer>
    );
  }
}
