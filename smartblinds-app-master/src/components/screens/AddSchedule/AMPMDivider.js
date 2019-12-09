import React from 'react'
import { View, Text } from 'react-native'
import styled from 'styled-components';
import config from '../../../config/config';

const { theme } = config;

const AMPMContainer = styled.View`
  position: absolute;
  flex: 1;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  justify-content: center;
  align-items: center;
`;

const InnerCircle = styled.View`
  justify-content: center;
  align-items: flex-end;
  flex-direction: row;
  width: 160px;
  height: 240px;
  margin-top: 38px;
`;

const Side = styled.View`
  justify-content: center;
  padding-left: 40px;
  padding-right: 40px;
  height: 170px;
  border-color: ${theme.font.transparent};
`;

const AMPMText = styled.Text`
  color: ${theme.font.transparent};
`;

const VerticalDivider = styled.View`
  border-bottom-width: 1px;
  border-bottom-color: #d8d8d8;
`;

const AMPMDivider = () => {
  return (
    <AMPMContainer>
      <InnerCircle>
        <Side>
          <AMPMText>PM</AMPMText>
        </Side>
        <VerticalDivider />
        <Side style={{ alignItems: 'flex-end', borderLeftWidth: 1 }}>
          <AMPMText>AM</AMPMText>
        </Side>
      </InnerCircle>
    </AMPMContainer>
  )
}

export default AMPMDivider
