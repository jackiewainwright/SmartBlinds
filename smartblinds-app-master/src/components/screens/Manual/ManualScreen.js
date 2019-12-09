import React from 'react';
import { observer } from 'mobx-react';
import { StyleSheet, Text, Switch } from 'react-native';
import styled from 'styled-components';

import config from '../../../config/config';
import { ViewWrapper, Header } from '../../layout';
import { inject } from 'mobx-react/native';

const { theme } = config;

const SwitchContainer = styled.View`
  flex: 1;
  justify-content: center;
  align-items: center;
`;

const BlindsSwitchContainer = styled.View`
  padding-top: 70px;
  padding-bottom: 70px;
`;

const BlindsSwitch = styled.Switch`
  transform: scaleX(3) scaleY(3) rotate(-90deg);
`;

const StateLabel = styled.Text`
  color: ${props => props.active ? theme.font.normal : theme.font.transparent };
`;

@inject('blindsStore')
@observer
class ManualScreen extends React.Component {
  static navigationOptions = {
    header: null,
  };

  constructor(props) {
    super(props);
    this.state = { switchState: props.blindsStore.blindsState === 'UP' ? true : false };
  }
  
  componentDidMount() {
    this.props.navigation.addListener('willFocus', async (route) => {
      console.log('Manual screen focused, fetching blinds state');
      await this.props.blindsStore.fetchBlindsInfo();
      this.syncSwitchState();
    });
  }

  syncSwitchState = () => {
    this.setState({
      switchState: this.props.blindsStore.blindsState === 'UP' ? true : false
    });
  }

  handleSwitchChange = value => {
    const blindsState = value === true ? 'UP' : 'DOWN';
    this.setState({ switchState: value });
    this.props.blindsStore.setBlindsState(blindsState);
  };

  renderSwitch = () => {
    const { blindsState } = this.props.blindsStore;

    if (blindsState === null) {
      return null;
    }

    return (
      <SwitchContainer>
        <StateLabel active={blindsState === 'UP'}>Up</StateLabel>

        <BlindsSwitchContainer>
          <BlindsSwitch
            thumbColor={theme.color.primary}
            trackColor={{
              true: theme.color.primaryLighter
            }}
            value={this.state.switchState}
            onValueChange={this.handleSwitchChange}
          />
        </BlindsSwitchContainer>

        <StateLabel active={blindsState === 'DOWN'}>Down</StateLabel>
      </SwitchContainer>
    )
  };

  render() {
    return (
      <ViewWrapper>
        <Header />
        {this.renderSwitch()}
      </ViewWrapper>
    );
  }
}

export default ManualScreen;
