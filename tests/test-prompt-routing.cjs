/* SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
 * SPDX-License-Identifier: MPL-2.0 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const source = fs.readFileSync(path.join(__dirname, '../import/popups/PopupOpener.qml'), 'utf8');
const names = ['message', 'getCheckbox', 'getCheckboxValue', 'getButtonStringKeys',
  'getMenuList', 'alert', 'confirm', 'prompt', 'selector', 'openPromptPopup', 'bindPromptPopup'];
const functions = names.map(name => {
  const match = source.match(new RegExp('^    function ' + name + '\\([^\\n]*\\) \\{[\\s\\S]*?^    \\}', 'm'));
  assert.ok(match, name);
  return match[0];
}).join('\n');
function signal() {
  const callbacks = new Set();
  return { connect: fn => callbacks.add(fn), disconnect: fn => callbacks.delete(fn),
    emit: () => [...callbacks].forEach(fn => fn()), count: () => callbacks.size };
}
for (const type of ['alert', 'confirm', 'prompt', 'select']) {
  for (const timing of ['accept', 'reject', 'abort-before-create', 'abort-while-busy', 'abort-active']) {
    const replies = [];
    let opening;
    const context = vm.createContext({
      console, _promptRequests: {}, _popupObject: null,
      pageStack: { busy: false, busyChanged: signal() },
      PageStatus: { Active: 1 },
      handlesMessage: () => true, aboutToOpenPopup() {},
      contentItem: { sendAsyncMessage: (topic, data) => replies.push({ topic, data }) },
      openPopupByTopic: (topic, subtopic, props, accept, reject, request) => {
        opening = { accept, reject, request };
      },
    });
    vm.runInContext(functions, context);
    const data = { winId: 42, promptId: '17', text: 'Prompt', inputs: [{value: 'abc'}] };
    context.message('embed:' + type, data);
    assert.ok(opening);
    // A later target must not receive this request's answer.
    context.contentItem = { sendAsyncMessage() { assert.fail('reply changed target'); } };
    context.message('embed:promptabort', { winId: 99, promptId: '17' });
    context.message('embed:promptabort', { winId: 42, promptId: '18' });
    assert.equal(opening.request.cancelled, false);
    if (timing === 'abort-before-create') context.message('embed:promptabort', data);
    const popup = { status: 1, statusChanged: signal(), value: 'typed',
      selectedIndex: 2, preventDialogsValue: false,
      reject: () => opening.reject(popup) };
    context.pageStack.busy = timing === 'abort-while-busy';
    context.bindPromptPopup(opening.request, popup, true);
    if (timing === 'accept') opening.accept(popup);
    else if (timing === 'reject') opening.reject(popup);
    else if (timing !== 'abort-before-create') context.message('embed:promptabort', data);
    if (timing === 'abort-while-busy') {
      assert.equal(replies.length, 0);
      context.pageStack.busy = false;
      context.pageStack.busyChanged.emit();
    }
    assert.equal(replies.length, 1, type + '/' + timing);
    assert.equal(replies[0].data.promptId, '17');
    assert.equal(replies[0].data.winId, 42);
    if (type === 'confirm' || type === 'prompt') {
      assert.equal(replies[0].data.accepted, timing === 'accept');
    }
    if (type === 'select') assert.equal(replies[0].data.button, timing === 'accept' ? 0 : 1);
    opening.accept(popup);
    assert.equal(replies.length, 1, 'duplicate callbacks must not answer a later prompt');
    assert.equal(context.pageStack.busyChanged.count(), 0);
    assert.equal(Object.keys(context._promptRequests).length, 0);
  }
}
console.log('Popup routing: 20 acceptance, rejection and teardown cases passed');
