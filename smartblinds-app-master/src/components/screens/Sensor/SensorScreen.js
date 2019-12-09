import React from 'react';
import { Text, Dimensions, View } from 'react-native';
import { Ionicons } from '@expo/vector-icons';
import styled from 'styled-components';
import { Subject } from 'rxjs';
import { debounceTime, tap } from 'rxjs/operators';

import { Header, ViewWrapper } from '../../layout';
import config from '../../../config/config';
import { inject } from 'mobx-react/native';

const { theme } = config;

const sliderWidth = Dimensions.get('window').width;

const SensorSlider = styled.Slider`
  transform: rotate(-90deg);
  width: 100%;
  height: ${sliderWidth};
`;

const SensorSliderContainer = styled.View`
  flex: 1;
  justify-content: center;
  align-items: center;
`;

const SensorValueText = styled.Text`
  color: ${theme.font.normal};
  font-size: 20px;
`;

const LightIcon = ({ type }) => (
  <View style={{ marginTop: 10, marginBottom: 10 }}>
    <Ionicons 
      name="ios-sunny"
      color={theme.font.transparent}
      size={type === 'big' ? 34 : 18}
    />
  </View>
);

@inject('blindsStore')
export default class SensorScreen extends React.Component {
  sliderChange$ = new Subject();

  static navigationOptions = {
    header: null,
  };

  constructor(props) {
    super(props);

    const sensorPreference = props.blindsStore.sensorPreference;
    this.state = {
      sensorPreference,
      presentSensorPreferenceValue: sensorPreference
    }
  }

  componentDidMount() {
    this.props.navigation.addListener('willFocus', async (route) => {
      console.log('Sensor screen focused, fetching sensor preference');
      await this.props.blindsStore.fetchBlindsInfo();
      await this.props.blindsStore.fetchLightSensorPreference();
      this.syncSensorPreference();
    });

    this.sliderChange$
    .pipe(
      tap(value => this.setState({ presentSensorPreference: value })),
      debounceTime(500)
    )
    .subscribe(this.handleSliderChange);
  }

  handleSliderChange = async value => {
    await this.props.blindsStore.setLightSensorPreference(value);
    this.syncSensorPreference();
  };

  syncSensorPreference = () => {
    const sensorPreference = this.props.blindsStore.sensorPreference;
    this.setState({
      sensorPreference,
      presentSensorPreference: sensorPreference
    });
  }

  renderSlider = () => {
    const { sensorPreference } = this.state;

    if (sensorPreference === null) {
      return null;
    }

    return (
      <SensorSliderContainer>
        <LightIcon type="big" />
        <SensorSlider
          step={5}
          value={sensorPreference}
          minimumValue={0}
          maximumValue={100}
          minimumTrackTintColor={theme.color.primaryLighter}
          thumbTintColor={theme.color.primary}
          onValueChange={value => this.sliderChange$.next(value)}
        />
        <LightIcon type="small" />

        <View style={{ marginTop: 20, alignItems: 'center' }}>
          <SensorValueText>
            Preference: {this.state.presentSensorPreference}%
          </SensorValueText>
          <SensorValueText>
            Current light reading: {this.props.blindsStore.lightReading}%
          </SensorValueText>
        </View>
      </SensorSliderContainer>
    );
  };

  render() {
    return (
      <ViewWrapper>
        <Header />
        {this.renderSlider()}
      </ViewWrapper>
    );
  }
}
