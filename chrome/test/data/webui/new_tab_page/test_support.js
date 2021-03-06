// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import {BrowserProxy} from 'chrome://new-tab-page/new_tab_page.js';
import {getDeepActiveElement} from 'chrome://resources/js/util.m.js';
import {keyDownOn} from 'chrome://resources/polymer/v3_0/iron-test-helpers/mock-interactions.js';
import {TestBrowserProxy} from 'chrome://test/test_browser_proxy.m.js';

/** @type {string} */
export const NONE_ANIMATION = 'none 0s ease 0s 1 normal none running';

/**
 * @param {!HTMLElement} element
 * @param {string} key
 */
export function keydown(element, key) {
  keyDownOn(element, '', [], key);
}

/**
 * Asserts the computed style value for an element.
 * @param {!HTMLElement} element The element.
 * @param {string} name The name of the style to assert.
 * @param {string} expected The expected style value.
 */
export function assertStyle(element, name, expected) {
  const actual = window.getComputedStyle(element).getPropertyValue(name).trim();
  assertEquals(expected, actual);
}

/**
 * Asserts the computed style for an element is not value.
 * @param {!HTMLElement} element The element.
 * @param {string} name The name of the style to assert.
 * @param {string} not The value the style should not be.
 */
export function assertNotStyle(element, name, not) {
  const actual = window.getComputedStyle(element).getPropertyValue(name).trim();
  assertNotEquals(not, actual);
}

/**
 * Asserts that an element is focused.
 * @param {!HTMLElement} element The element to test.
 */
export function assertFocus(element) {
  assertEquals(element, getDeepActiveElement());
}

/**
 * Creates a mocked test proxy.
 * @return {TestBrowserProxy}
 */
export function createTestProxy() {
  const testProxy = TestBrowserProxy.fromClass(BrowserProxy);
  testProxy.callbackRouter = new newTabPage.mojom.PageCallbackRouter();
  testProxy.callbackRouterRemote =
      testProxy.callbackRouter.$.bindNewPipeAndPassRemote();
  testProxy.handler =
      TestBrowserProxy.fromClass(newTabPage.mojom.PageHandlerRemote);
  testProxy.setResultFor('createIframeSrc', '');
  return testProxy;
}

/** @return {!newTabPage.mojom.Theme} */
export function createTheme() {
  const searchBox = {
    bg: {value: 0xff000000},
    icon: {value: 0xff000001},
    iconSelected: {value: 0xff000002},
    placeholder: {value: 0xff000003},
    resultsBg: {value: 0xff000004},
    resultsBgHovered: {value: 0xff000005},
    resultsBgSelected: {value: 0xff000006},
    resultsDim: {value: 0xff000007},
    resultsDimSelected: {value: 0xff000008},
    resultsText: {value: 0xff000009},
    resultsTextSelected: {value: 0xff00000a},
    resultsUrl: {value: 0xff00000b},
    resultsUrlSelected: {value: 0xff00000c},
    text: {value: 0xff00000d},
  };
  return {
    type: newTabPage.mojom.ThemeType.DEFAULT,
    info: {chromeThemeId: 0},
    backgroundColor: {value: 0xffff0000},
    shortcutBackgroundColor: {value: 0xff00ff00},
    shortcutTextColor: {value: 0xff0000ff},
    isDark: false,
    logoColor: null,
    backgroundImage: null,
    backgroundImageAttribution1: '',
    backgroundImageAttribution2: '',
    backgroundImageAttributionUrl: null,
    dailyRefreshCollectionId: '',
    searchBox: searchBox,
    shortcutUseWhiteAddIcon: false,
    shortcutUseTitlePill: false,
  };
}
