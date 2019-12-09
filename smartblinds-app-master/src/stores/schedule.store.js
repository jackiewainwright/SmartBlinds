import { observable, action } from 'mobx';

export default class ScheduleStore {
  @observable schedules = [];

  constructor(scheduleService) {
    this.scheduleService = scheduleService;
  }

  @action
  fetchAllSchedules = async () => {
    this.schedules = await this.scheduleService.fetchAllSchedules();
  }

  @action
  deleteSchedule = async scheduleId => {
    this.scheduleService.deleteSchedule(scheduleId);
    this.schedules = this.schedules.filter(schedule => schedule.scheduleId !== scheduleId);
  };

  @action
  setSchedule = async ({ start, end, days }) => {
    await this.scheduleService.setSchedule({ start, end, days });
  };

  @action
  updateScheduleDays = async (scheduleId, days) => {
    const updatedSchedule = await this.scheduleService.updateScheduleDays(scheduleId, days);
    const scheduleToUpdate = this.schedules.find(schedule => schedule.scheduleId === scheduleId);
    scheduleToUpdate.days = updatedSchedule.days;
  };
}
