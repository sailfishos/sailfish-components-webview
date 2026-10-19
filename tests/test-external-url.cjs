/* SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
 * SPDX-License-Identifier: MPL-2.0 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const source = fs.readFileSync(path.join(__dirname, '../import/webview/ExternalUrlHandler.qml'), 'utf8');
const functions = ['current', 'respond', 'dispatch', 'receive', 'nextPrompt', 'discardStaleRequests']
    .map(name => {
        const match = source.match(new RegExp('^    function ' + name + '\\([^\\n]*\\) \\{[\\s\\S]*?^    \\}', 'm'));
        assert.ok(match, name);
        return match[0];
    }).join('\n');
let cases = 0;
function fixture(options = {}) {
    const replies = [], launched = [], dialogs = [];
    let now = 1000;
    const tabs = [{tabId: '3', locationRevision: '7'}, {tabId: '4', locationRevision: '2'}];
    const scope = vm.createContext({
        Date: {now: () => now}, requests: {}, prompt: null, promptComponent: {},
        PageStatus: {Active: 2},
        Qt: {openUrlExternally: url => { launched.push(url); return options.delivered !== false; }},
        expiryTimer: {running: false, start() {this.running = true;}, stop() {this.running = false;}},
        webView: {
            tabModel: {snapshot: () => tabs},
            sendAsyncMessageToTab: (tab, topic, data) => replies.push({tab, topic, ...data}),
        },
        pageStack: options.noPageStack ? null : {
            busy: !!options.busy,
            push(component, properties) {
                const dialog = {...properties, status: 2,
                    reject() { scope.respond(this.request, 'declined'); close(); }};
                dialogs.push(dialog);
                return options.pushFails ? null : dialog;
            },
        },
    });
    vm.runInContext(functions, scope);
    function close() { scope.prompt = null; scope.nextPrompt(); }
    function receive(data = {}, tab = '3') {
        scope.receive(tab, {id: 'a', winId: 11, locationRevision: '7',
            url: 'mailto:test@example.org', host: 'https://example.org',
            canRemember: true, ...data});
    }
    function accept(remember = false, dialog = dialogs.at(-1)) {
        scope.dispatch(dialog.request, remember); close();
    }
    function reject() { dialogs.at(-1).reject(); }
    cases++;
    return {scope, replies, launched, dialogs, tabs, receive, accept, reject,
        expire() { now += 110001; scope.discardStaleRequests(); }};
}
for (const url of ['mailto:test@example.org', 'tel:+123', 'custom-app:callback']) {
    const f = fixture();
    f.receive({url, permissionAllowed: true});
    assert.deepEqual(f.launched, [url]);
    assert.equal(f.dialogs.length, 0);
    assert.equal(f.replies[0].tab, '3');
    assert.equal(f.replies[0].winId, 11);
    assert.equal(f.replies[0].id, 'a');
    assert.equal(f.replies[0].topic, 'externalurlresponse');
    assert.equal(f.replies[0].outcome, 'dispatched');
    assert.equal(f.replies[0].remember, false);
}
{
    const f = fixture(); f.receive();
    assert.equal(f.launched.length, 0, 'A site without permission must prompt');
    f.accept(true);
    assert.equal(f.launched.length, 1);
    assert.equal(f.replies[0].remember, true);
    assert.equal(f.scope.expiryTimer.running, false);
    f.scope.dispatch(f.dialogs[0].request, true);
    assert.equal(f.launched.length, 1, 'Duplicate acceptance cannot dispatch twice');
}
{
    const f = fixture(); f.receive(); f.reject();
    assert.equal(f.launched.length, 0);
    assert.equal(f.replies[0].outcome, 'declined');
}
for (const data of [{privateBrowsing: true}, {canRemember: false},
    {url: 'intent://maps/#Intent;scheme=https;end', permissionAllowed: true}]) {
    const f = fixture(); f.receive(data);
    assert.equal(f.dialogs.length, 1);
    f.accept(true);
    assert.equal(f.replies[0].remember, false);
}
{
    const f = fixture({delivered: false}); f.receive(); f.accept(true);
    assert.equal(f.replies[0].outcome, 'unsupported');
    assert.equal(f.replies[0].remember, false);
}
for (const url of ['https://example.org', 'http://example.org', 'file:///tmp/test',
    'JaVaScRiPt:alert(1)', 'data:text/html,test', 'about:blank', 'chrome:test',
    'resource:test', 'missing-scheme']) {
    const f = fixture(); f.receive({url, permissionAllowed: true});
    assert.equal(f.launched.length, 0);
    assert.equal(f.dialogs.length, 0);
    assert.equal(f.replies[0].outcome, 'unsupported');
}
for (const mutation of ['navigate', 'close', 'expire']) {
    const f = fixture(); f.receive();
    const dialog = f.dialogs[0];
    if (mutation === 'navigate') f.tabs[0].locationRevision = '8';
    if (mutation === 'close') f.tabs.shift();
    if (mutation === 'expire') f.expire();
    else f.scope.discardStaleRequests();
    f.scope.dispatch(dialog.request, true);
    assert.equal(f.launched.length, 0, mutation + ' must cancel dispatch');
    assert.equal(f.replies.length, 1);
    assert.equal(f.replies[0].outcome, 'cancelled');
}
{
    const f = fixture(); f.receive();
    f.tabs[0].locationRevision = '8';
    f.accept(true); // Even if revisionChanged has not yet been delivered.
    assert.equal(f.launched.length, 0);
    assert.equal(f.replies[0].outcome, 'cancelled');
}
{
    const f = fixture(); f.receive(); f.receive();
    assert.equal(f.dialogs.length, 1, 'Repeated messages share the original request');
    f.receive({id: 'b', url: 'tel:+123'});
    assert.equal(f.replies[0].outcome, 'cancelled');
    f.accept();
    assert.deepEqual(f.launched, ['tel:+123']);
    assert.equal(f.replies.at(-1).id, 'b');
}
{
    const f = fixture({busy: true}); f.receive();
    f.receive({id: 'b', locationRevision: '2', url: 'tel:+123'}, '4');
    assert.equal(f.dialogs.length, 0);
    f.scope.pageStack.busy = false; f.scope.nextPrompt(); f.accept(); f.accept();
    assert.deepEqual(f.replies.map(r => r.tab), ['3', '4']);
}
{
    const f = fixture({busy: true}); f.receive(); f.expire();
    f.scope.pageStack.busy = false; f.scope.nextPrompt();
    assert.equal(f.dialogs.length, 0);
    assert.equal(f.replies[0].outcome, 'cancelled');
}
for (const options of [{noPageStack: true}, {pushFails: true}]) {
    const f = fixture(options); f.receive();
    assert.equal(f.launched.length, 0);
    assert.equal(f.replies[0].outcome, 'cancelled');
}
console.log(`External URL routing: ${cases} permission, dispatch, tab identity and cancellation cases passed`);

// Optional integration with the real patched Gecko chooser, without requiring
// a Gecko checkout for the standalone WebView regression suite.
if (process.env.GECKO_CHOOSER_SOURCE) {
    const chooserSource = fs.readFileSync(process.env.GECKO_CHOOSER_SOURCE, 'utf8')
        .replace(/^import .*;\n/gm, '').replace('export class', 'class')
        .split('nsContentDispatchChooser.prototype')[0];
    (async () => {
        for (const scenario of ['allowed', 'accept', 'decline', 'navigate']) {
            const f = fixture();
            let listener, timer, legacyLaunches = 0;
            const sendResponse = f.scope.webView.sendAsyncMessageToTab;
            f.scope.webView.sendAsyncMessageToTab = (tab, topic, data) => {
                sendResponse(tab, topic, data);
                listener.onMessageReceived(topic, JSON.stringify(data));
            };
            const service = {
                getIDByBrowsingContext: () => 11,
                addMessageListener: (topic, value) => { listener = value; },
                removeMessageListener: () => { listener = null; },
                sendAsyncMessage: (id, topic, data) => {
                    assert.equal(topic, 'embed:externalurl');
                    f.receive(JSON.parse(data));
                },
            };
            const Clazz = new Function('XPCOMUtils', 'AppConstants', 'E10SUtils', 'Cc',
                'Ci', 'ChromeUtils', 'Services', 'clearTimeout', 'setTimeout',
                chooserSource + '\nreturn nsContentDispatchChooser;')(
                {defineLazyPreferenceGetter: (o, k, p, v) => {o[k] = v;}},
                {MOZ_WIDGET_TOOLKIT: 'qt'}, {STANDARD_SAFE_PROTOCOLS: ['http', 'https', 'mailto']},
                {'@mozilla.org/embedlite-app-service;1': {getService: () => service}}, {},
                {generateQI: () => () => {}},
                {perms: {ALLOW_ACTION: 1, testPermissionFromPrincipal: () => scenario === 'allowed' ? 1 : 0}},
                () => {timer = null;}, callback => {timer = callback; return 1;});
            const chooser = new Clazz();
            chooser._isSupportedPrincipal = () => true;
            const context = {isDiscarded: false, usePrivateBrowsing: false,
                currentWindowGlobal: {documentURI: {spec: 'https://example.org'}}};
            const done = chooser.handleURI({launchWithURI: () => {legacyLaunches++;}},
                {scheme: 'mailto', spec: 'mailto:test@example.org'},
                {exposablePrePath: 'https://example.org'}, context, false, true);
            if (scenario === 'accept') f.accept();
            if (scenario === 'decline') f.reject();
            if (scenario === 'navigate') {
                f.tabs[0].locationRevision = '8';
                context.currentWindowGlobal = {};
                f.scope.discardStaleRequests();
            }
            assert.equal(timer, null, 'WebView must answer before the Gecko timeout');
            await done;
            assert.equal(listener, null, 'Gecko must release its response listener');
            assert.equal(legacyLaunches, 0, 'Only one dispatcher owns the request');
            assert.equal(f.launched.length, ['allowed', 'accept'].includes(scenario) ? 1 : 0);
        }
        console.log('Patched Gecko/WebView integration: 4 handoff and cancellation cases passed');
    })().catch(error => { console.error(error); process.exitCode = 1; });
}
