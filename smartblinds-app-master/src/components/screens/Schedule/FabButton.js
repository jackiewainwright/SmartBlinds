import React from 'react';
import { View, Text } from 'react-native';
import { Ionicons } from '@expo/vector-icons';
import styled from 'styled-components';

const FabButtonContainer = styled.TouchableOpacity`
  width: 60px;
  height: 60px;
  background-color: ${props => props.color};
  border-radius: 100;
  align-items: center;
  justify-content: center;
  box-shadow: 1px 2px 3px rgba(0, 0, 0, 0.4);
  elevation: 6;
`;

const FabButton = ({ onPress, icon, color }) => {
  return (
    <FabButtonContainer
        onPress={onPress}
        color={color}
    >
      <Ionicons color="#ffffff" name={icon} size={34} />
    </FabButtonContainer>
  )
}

export default FabButton
