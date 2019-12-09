import * as axios from 'axios';
import config from '../config/config';
import { Localization } from 'expo';
import { changeTimezone } from '../utils/time.utils';
import moment from 'moment-timezone';

const BASE_URL = `http://${config.server.host}/schedule`;

export default class ScheduleService {
  async fetchAllSchedules() {
    try {
      const result = await axios.get(`${BASE_URL}/`);
      return result.data;
    } catch (error) {
      alert(`Cannot fetch schedules - ${error}`);
    }
  }

  async deleteSchedule(scheduleId) {
    try {
      const result = await axios.delete(`${BASE_URL}/${scheduleId}`);
      return result.data;
    } catch (error) {
      alert(`Cannot delete schedule - ${error}`);
    }
  }

  async setSchedule({ start, end, days }) {
    const startDate = moment().tz(Localization.timezone);
    const endDate = moment().tz(Localization.timezone);

    startDate.set({
      hour: start.hour,
      minute: start.minute,
      second: 0,
      milisecond: 0
    });

    endDate.set({
      hour: end.hour,
      minute: end.minute,
      second: 0,
      milisecond: 0
    });

    const payload = {
      from: new Date(startDate.format()).toISOString(),
      to: new Date(endDate.format()).toISOString(),
      days
    };

    try {
      const result = await axios.post(`${BASE_URL}/`, payload);
      return result.data;
    } catch (error) {
      alert(`Cannot set schedules - ${error}`);
    }
  }

  async updateScheduleDays(scheduleId, days) {
    try {
      const result = await axios.put(`${BASE_URL}/${scheduleId}/days`, { days });
      return result.data; 
    } catch (error) {
      alert(`Cannot update schedule days - ${error}`);
    }
  }
}
