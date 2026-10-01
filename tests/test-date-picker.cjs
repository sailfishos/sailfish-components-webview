/* SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
 * SPDX-License-Identifier: MPL-2.0 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');

const source = fs.readFileSync(path.join(__dirname,
  '../import/pickers/WebDatePickerDialog.qml'), 'utf8');
const names = ['_number', '_dateMilliseconds', '_localDate', '_valueMilliseconds',
  '_nearestSelectable', '_initialDate', '_isSelectable'];
const functions = names.map(name => {
  const match = source.match(new RegExp('^    function ' + name +
    '\\([^\\n]*\\) \\{[\\s\\S]*?^    \\}', 'm'));
  assert.ok(match, name);
  return match[0];
}).join('\n');
const DAY = 86400000;
const start = Date.UTC(2026, 8, 17);
function picker(values) {
  const context = vm.createContext({ initialValue: '2026-09-17', dateTime: false,
    minimumValue: null, maximumValue: null, timeMinimum: null, timeMaximum: null,
    stepValue: null, stepBase: null,
    ...values });
  vm.runInContext(functions, context);
  return context;
}
function date(year, month, day) {
  const result = new Date(0);
  result.setFullYear(year, month - 1, day);
  result.setHours(12, 0, 0, 0);
  return result;
}

const native = picker({ minimumValue: start, maximumValue: start + 2 * DAY,
  stepValue: 2 * DAY, stepBase: start });
assert.equal(native._isSelectable(date(2026, 9, 17)), true);
assert.equal(native._isSelectable(date(2026, 9, 18)), false);
assert.equal(native._isSelectable(date(2026, 9, 19)), true);
assert.equal(native._isSelectable(date(2026, 9, 20)), false);

const datetime = picker({ dateTime: true, minimumValue: start,
  maximumValue: start + 2 * DAY, timeMinimum: start + DAY / 2,
  timeMaximum: start + 2 * DAY + 18 * 3600000, stepValue: 2 * DAY,
  stepBase: start + DAY / 2 });
assert.equal(datetime._isSelectable(date(2026, 9, 17)), true);
assert.equal(datetime._isSelectable(date(2026, 9, 18)), false);
assert.equal(datetime._isSelectable(date(2026, 9, 19)), true);
assert.equal(datetime._isSelectable(date(2026, 9, 20)), false);
datetime.initialValue = '2026-09-19';
const initial = datetime._initialDate();
assert.equal(initial.getDate(), 19, 'A valid maximum day stays selected');
assert.equal(datetime._isSelectable(initial), true);

for (const step of [60000, 37 * 60000, null]) {
  const fractionalDay = picker({ dateTime: true, stepValue: step,
    stepBase: start + 123456 });
  assert.equal(fractionalDay._isSelectable(date(2026, 9, 17)), true);
}
const beforeEpoch = picker({ dateTime: true, initialValue: '1969-12-31',
  minimumValue: -DAY, maximumValue: -DAY, timeMinimum: -DAY,
  timeMaximum: -1, stepValue: DAY, stepBase: -DAY / 2 });
assert.equal(beforeEpoch._isSelectable(beforeEpoch._initialDate()), true);
assert.equal(beforeEpoch._initialDate().getFullYear(), 1969);
const ancient = picker({ initialValue: '0001-01-01' });
assert.equal(ancient._initialDate().getFullYear(), 1);
assert.ok(Number.isNaN(ancient._dateMilliseconds(2026, 2, 30)));
console.log('Native date/datetime calendar constraints and initial selection passed');

const timeSource = fs.readFileSync(path.join(__dirname,
  '../import/pickers/WebTimePickerDialog.qml'), 'utf8');
const timeFunctions = ['_number', '_dayStart', '_valueMilliseconds', '_initialTime',
  '_isSelectable'].map(name => timeSource.match(new RegExp('^    function ' + name +
    '\\([^\\n]*\\) \\{[\\s\\S]*?^    \\}', 'm'))[0]).join('\n');
function clock(values) {
  const context = vm.createContext({ dateTime: false, timeValue: '12:34',
    timeMinimum: null, timeMaximum: null, stepValue: 60000, stepBase: 0,
    ...values });
  vm.runInContext(timeFunctions, context);
  const initial = context._initialTime();
  context._second = Math.floor(initial / 1000) % 60;
  context._millisecond = initial % 1000;
  return { context, initial };
}
let time = clock({ timeValue: '12:34:56.789', stepValue: 1 });
assert.equal(time.initial, 45296789);
assert.equal(time.context._isSelectable(14, 30), true);
assert.equal(time.context._second, 56);
assert.equal(time.context._millisecond, 789);
time = clock({ timeValue: '08:00', timeMinimum: 9 * 3600000,
  timeMaximum: 17 * 3600000, stepValue: 900000 });
assert.equal(time.initial, 9 * 3600000);
assert.equal(time.context._isSelectable(8, 59), false);
assert.equal(time.context._isSelectable(17, 0), true);
assert.equal(time.context._isSelectable(17, 1), false);
assert.equal(time.context._isSelectable(12, 16), false);
time = clock({ timeMinimum: 22 * 3600000, timeMaximum: 2 * 3600000 });
assert.equal(time.initial, 22 * 3600000);
assert.equal(time.context._isSelectable(23, 30), true);
assert.equal(time.context._isSelectable(1, 30), true);
assert.equal(time.context._isSelectable(12, 0), false);
time = clock({ dateTime: true, selectedDate: date(2026, 9, 19),
  timeValue: '2026-09-17T12:00', timeMinimum: start + DAY / 2,
  timeMaximum: start + 2 * DAY + 18 * 3600000,
  stepValue: 2 * DAY, stepBase: start + DAY / 2 });
assert.equal(time.initial, DAY / 2);
assert.equal(time.context._isSelectable(12, 0), true);
assert.equal(time.context._isSelectable(12, 1), false);
time = clock({ dateTime: true, selectedDate: date(1969, 12, 31),
  timeValue: '1969-12-31T23:59:59.9', stepValue: 100 });
assert.equal(time.initial, DAY - 100);
assert.equal(time.context._second, 59);
assert.equal(time.context._millisecond, 900);
console.log('Native clock precision, bounds, overnight ranges and step constraints passed');

// A valid datetime day must survive initialization even when midnight is off-step.
for (const step of [DAY, 37 * 60000]) {
  const calendar = picker({ dateTime: true, initialValue: '2026-09-17',
    timeMinimum: null, timeMaximum: null, stepValue: step, stepBase: start + DAY / 2 });
  assert.equal(calendar._initialDate().getDate(), 17);
}
// A boundary day needs a step inside the actual time bounds, not merely that day.
const bounded = picker({ dateTime: true, initialValue: '2026-09-18',
  timeMinimum: start + DAY / 2, timeMaximum: start + DAY + 10 * 3600000,
  minimumValue: start, maximumValue: start + DAY, stepValue: DAY,
  stepBase: start + DAY / 2 });
assert.equal(bounded._isSelectable(date(2026, 9, 17)), true);
assert.equal(bounded._isSelectable(date(2026, 9, 18)), false);
assert.equal(bounded._initialDate().getDate(), 17);
assert.equal(bounded._isSelectable(bounded._initialDate()), true);
bounded.timeMinimum = start + 13 * 3600000;
assert.equal(bounded._isSelectable(date(2026, 9, 17)), false);

// Execute the production signal handlers as well as the constraint functions.
function handler(qml, name) {
  const match = qml.match(new RegExp('^    ' + name + ': \\{([\\s\\S]*?)^    \\}', 'm'));
  assert.ok(match, name);
  return match[1];
}
function productionFunctions(qml) {
  return [...qml.matchAll(/^    function \w+\([^\n]*\) \{[\s\S]*?^    \}/gm)]
    .map(match => match[0]).join('\n');
}
const replies = [];
let releases = 0;
const request = { active: true, release() { ++releases; } };
const contentItem = { sendAsyncMessage(topic, data) { replies.push({ topic, data }); } };
function timePage(dateTime = true) {
  const page = vm.createContext({ dateTime, selectedDate: date(2026, 9, 17),
    timeValue: '2026-09-17T19:00', timeMinimum: start + 18 * 3600000,
    timeMaximum: start + DAY + 12 * 3600000, stepValue: 60000, stepBase: 0,
    requestState: null, contentItem: null, _completed: false,
    winId: 42, requestId: 'test', clock: {} });
  vm.runInContext(productionFunctions(timeSource), page);
  page.initialize(page.selectedDate);
  return page;
}
const calendar = vm.createContext({ dateTime: true, selectedDate: date(2026, 9, 18),
  requestState: request, contentItem, _completed: false });
vm.runInContext(productionFunctions(source), calendar);
// Silica prepares the forward page, then destroys it to open the year menu.
let preview = timePage();
assert.equal(preview.clock.hour, 19);
vm.runInContext(handler(timeSource, 'Component.onDestruction'), preview);
assert.equal(replies.length, 0);
assert.equal(releases, 0);
// Returning from the year menu recreates the preview with the old initial date.
preview = timePage();
calendar.acceptDestinationInstance = preview;
vm.runInContext(handler(source, 'onAccepted'), calendar);
assert.equal(preview.selectedDate.getDate(), 18);
assert.equal(preview.clock.hour, 12, 'Handoff resets the clock to the new day bounds');
assert.equal(preview.clock.minute, 0);
assert.equal(preview._isSelectable(preview.clock.hour, preview.clock.minute), true);
assert.equal(preview.requestState, request);
assert.equal(calendar.requestState, null);
vm.runInContext(handler(source, 'Component.onDestruction'), calendar);
assert.equal(releases, 0, 'Replacing the calendar leaves the clock request alive');
preview._finish(true);
vm.runInContext(handler(timeSource, 'Component.onDestruction'), preview);
assert.equal(replies.length, 1);
assert.equal(replies[0].data.accepted, true);
assert.equal(replies[0].data.day, 18);
assert.equal(replies[0].data.hour, 12);
assert.equal(releases, 1);
// Cancelling a standalone clock, or aborting during transition, answers once.
for (const abort of [false, true]) {
  const page = timePage(false);
  page.requestState = { active: !abort, release() { ++releases; } };
  page.contentItem = contentItem;
  if (abort) {
    page.status = 1;
    page.PageStatus = { Active: 1 };
    let pops = 0;
    page.pageStack = { busy: true, pop() { ++pops; } };
    page.closeCancelledRequest();
    assert.equal(pops, 0);
    page.pageStack.busy = false;
    page.closeCancelledRequest();
    assert.equal(pops, 1);
  } else {
    page._finish(false);
  }
  vm.runInContext(handler(timeSource, 'Component.onDestruction'), page);
}
assert.equal(replies.length, 3);
assert.equal(releases, 3);
assert.equal(replies[1].data.accepted, false);
assert.equal(replies[2].data.accepted, false);
const cancelledCalendar = vm.createContext({ dateTime: true, requestState: request,
  contentItem, _completed: false, winId: 42, requestId: 'cancel' });
vm.runInContext(productionFunctions(source), cancelledCalendar);
const cancelledPreview = timePage();
vm.runInContext(handler(source, 'onRejected'), cancelledCalendar);
vm.runInContext(handler(source, 'Component.onDestruction'), cancelledCalendar);
vm.runInContext(handler(timeSource, 'Component.onDestruction'), cancelledPreview);
assert.equal(replies.length, 4, 'Calendar cancellation and preview destruction answer once');
assert.equal(replies[3].data.accepted, false);
assert.equal(releases, 4);
console.log('Calendar boundary days, year-menu lifetime and clock handoff regressions passed');
