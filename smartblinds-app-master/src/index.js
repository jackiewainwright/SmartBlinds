import React from 'react';
import { KeepAwake, registerRootComponent } from 'expo';
import { Provider } from 'mobx-react/native';

import App from './App';
import ScheduleStore from './stores/schedule.store';
import BlindsStore from './stores/blinds.store';
import ScheduleService from './services/schedule.service';
import BlindsService from './services/blinds.service';

if (__DEV__) {
  KeepAwake.activate();
}

const services = {
  scheduleService: new ScheduleService(),
  blindsService: new BlindsService()
};

const stores = {
  blindsStore: new BlindsStore(services.blindsService),
  scheduleStore: new ScheduleStore(services.scheduleService)
};

const Root = () => (
  <Provider {...stores}>
    <App />
  </Provider>
);

registerRootComponent(Root);
