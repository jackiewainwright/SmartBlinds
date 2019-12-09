import { Localization } from 'expo';
import moment from 'moment-timezone';

function getTimeDisplayString({ h, m }) {
  m = padMinutes(m);
  const type = (h >= 12) ? 'PM' : 'AM';
  h = h >= 13 ? h - 12 : h;
  h = h === 0 ? 12 : h;
  return `${h}:${m} ${type}`;
}

function convertUTCToLocal(date) {
  const original = moment.utc(date).format();
  const converted = moment(original).tz(Localization.timezone);
  return converted;
}

function padMinutes(min) {
  if (`${min}`.length < 2) {
    return `0${min}`;
  }

  return min;
}

function anyDaySelected(days) {
  return Object.values(days).some(value => value === true);
}

export {
  getTimeDisplayString,
  convertUTCToLocal,
  padMinutes,
  anyDaySelected
};
