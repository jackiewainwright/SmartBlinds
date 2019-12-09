import React, { useState } from 'react';
import { Text, TouchableWithoutFeedback } from 'react-native';
import styled from 'styled-components';
import config from '../../../config/config';

const { theme } = config;

const DayLabel = styled.Text`
  padding: 6px;
  color: ${props => props.selected ? theme.font.normal : theme.font.transparent };
`;

const DayContainer = styled.TouchableWithoutFeedback``;

const Day = ({ name, selected, onPress }) => {
  const displayName = name.toUpperCase();

  return (
    <DayContainer onPress={onPress}>
      <DayLabel
        selected={selected}
      >
        {displayName}
      </DayLabel>
    </DayContainer>
  );
};

export default Day;
