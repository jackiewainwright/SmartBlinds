import React from 'react'
import { Text, View, TouchableOpacity } from 'react-native'
import { Ionicons } from '@expo/vector-icons';
import styled from 'styled-components';
import config from '../../../config/config';

const { theme } = config;

const ButtonContainer = styled.View`
  flex-direction: row;
  align-self: flex-start;
  padding: 8px;
`;

const IconContainer = styled.View`
  padding-right: 10px;
  justify-content: center;
`;

const LabelContainer = styled.View`
  justify-content: center;
`;

const Label = styled.Text`
  color: ${theme.font.transparent};
`;

const DeleteButton = ({ onPress }) => {
  return (
    <TouchableOpacity onPress={onPress} style={{ alignSelf: 'flex-start' }}>
      <ButtonContainer>
        <IconContainer>
          <Ionicons color={theme.font.transparent} name="ios-trash" size={18} />
        </IconContainer>
        <LabelContainer>
          <Label>Delete</Label>
        </LabelContainer>
      </ButtonContainer>
    </TouchableOpacity>
  )
}

export default DeleteButton;

