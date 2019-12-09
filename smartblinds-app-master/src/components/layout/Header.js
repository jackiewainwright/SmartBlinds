import React, { Component } from 'react'
import styled from 'styled-components';
import { Text } from 'react-native'
import config from '../../config/config';

const { theme } = config;

const HeaderComponent = styled.View`
  width: 100%;
  height: 60px;
  justify-content: center;
  align-items: center;
`;

const HeaderTitle = styled.Text`
  font-size: 16px;
  color: ${theme.font.normal};
`;

class Header extends Component {
  render() {
    return (
      <HeaderComponent>
        <HeaderTitle>
          SmartBlinds
        </HeaderTitle>
      </HeaderComponent>
    )
  }
}

export default Header;
