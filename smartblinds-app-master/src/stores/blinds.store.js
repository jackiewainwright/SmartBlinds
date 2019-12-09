import { observable, action } from 'mobx';

export default class BlindsStore {
  @observable blindsState = null;
  @observable sensorPreference = null;
  @observable lightReading = null;

  constructor(blindsService) {
    this.blindsService = blindsService;
    this.fetchBlindsInfo();
    this.fetchLightSensorPreference();
  }

  @action
  async fetchBlindsInfo() {
    const { state, light } = await this.blindsService.fetchBlindsInfo();
    this.blindsState = state;
    this.lightReading = light;
  }

  @action
  async fetchLightSensorPreference() {
    const preference = await this.blindsService.fetchLightSensorPreference();
    this.sensorPreference = preference;
  }

  @action
  async setBlindsState(state) {
    const newState = await this.blindsService.setBlindsState(state);
    this.blindsState = newState;
  }

  @action
  async setLightSensorPreference(value) {
    const newValue = await this.blindsService.setLightSensorPreference(value);
    this.sensorPreference = newValue;
  }
}
