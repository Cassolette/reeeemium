// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.tasks.tab_management;

import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.content.Context;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import org.chromium.chrome.browser.tasks.tab_management.suggestions.TabSuggestion;
import org.chromium.chrome.tab_ui.R;
import org.chromium.testing.local.LocalRobolectricTestRunner;
import org.chromium.ui.modelutil.PropertyModel;

import java.util.List;

/**
 * Unit tests for {@link MessageCardProviderMediator}.
 */
@RunWith(LocalRobolectricTestRunner.class)
public class MessageCardProviderMediatorUnitTest {
    private static final int SUGGESTED_TAB_COUNT = 2;
    private static final int TESTING_ACTION = -1;

    private MessageCardProviderMediator mMediator;

    @Mock
    private MessageCardView.DismissActionProvider mUiDismissActionProvider;

    @Mock
    private Context mContext;

    @Mock
    private TabSuggestionMessageService.TabSuggestionMessageData mTabSuggestionMessageData;

    @Before
    public void setUp() {
        MockitoAnnotations.initMocks(this);

        doNothing().when(mUiDismissActionProvider).dismiss(anyInt());
        mMediator = new MessageCardProviderMediator(mContext, mUiDismissActionProvider);
    }

    private void enqueueMessageItem(@MessageService.MessageType int type, int tabSuggestionAction) {
        switch (type) {
            case MessageService.MessageType.TAB_SUGGESTION:
                when(mTabSuggestionMessageData.getSize()).thenReturn(SUGGESTED_TAB_COUNT);
                when(mTabSuggestionMessageData.getActionType()).thenReturn(tabSuggestionAction);
                when(mTabSuggestionMessageData.getDismissActionProvider())
                        .thenReturn((messageType) -> {});
                when(mTabSuggestionMessageData.getReviewActionProvider()).thenReturn(() -> {});
                mMediator.messageReady(type, mTabSuggestionMessageData);
                break;
            default:
                mMediator.messageReady(type, new MessageService.MessageData() {});
        }
    }

    @Test
    public void getMessageItemsTest() {
        enqueueMessageItem(
                MessageService.MessageType.TAB_SUGGESTION, TabSuggestion.TabSuggestionAction.CLOSE);

        Assert.assertEquals(1, mMediator.getMessageItems().size());
        Assert.assertTrue(mMediator.getReadyMessageItemsForTesting().isEmpty());
        Assert.assertFalse(mMediator.getShownMessageItemsForTesting().isEmpty());
    }

    @Test
    public void getMessageItemsTest_TwoDifferentTypeMessage() {
        enqueueMessageItem(
                MessageService.MessageType.TAB_SUGGESTION, TabSuggestion.TabSuggestionAction.CLOSE);

        Assert.assertEquals(1, mMediator.getMessageItems().size());
        Assert.assertTrue(mMediator.getReadyMessageItemsForTesting().isEmpty());
        Assert.assertFalse(mMediator.getShownMessageItemsForTesting().isEmpty());

        enqueueMessageItem(MessageService.MessageType.FOR_TESTING, TESTING_ACTION);

        Assert.assertEquals(2, mMediator.getMessageItems().size());
        Assert.assertTrue(mMediator.getReadyMessageItemsForTesting().isEmpty());
        Assert.assertFalse(mMediator.getShownMessageItemsForTesting().isEmpty());
    }

    @Test
    public void getMessageItemsTest_OneMessageForEachMessageType() {
        enqueueMessageItem(
                MessageService.MessageType.TAB_SUGGESTION, TabSuggestion.TabSuggestionAction.CLOSE);
        enqueueMessageItem(
                MessageService.MessageType.TAB_SUGGESTION, TabSuggestion.TabSuggestionAction.GROUP);
        enqueueMessageItem(MessageService.MessageType.FOR_TESTING, TESTING_ACTION);

        List<MessageCardProviderMediator.Message> messages = mMediator.getMessageItems();
        Assert.assertEquals(2, messages.size());
        Assert.assertEquals(MessageService.MessageType.TAB_SUGGESTION, messages.get(0).type);
        Assert.assertEquals(MessageService.MessageType.FOR_TESTING, messages.get(1).type);

        Assert.assertEquals(2, mMediator.getShownMessageItemsForTesting().size());
        Assert.assertTrue(mMediator.getShownMessageItemsForTesting().containsKey(
                MessageService.MessageType.TAB_SUGGESTION));
        Assert.assertTrue(mMediator.getShownMessageItemsForTesting().containsKey(
                MessageService.MessageType.FOR_TESTING));
    }

    @Test
    public void getMessageItemsTest_ReturnFirstMessageFromMultipleSameTypeMessages() {
        enqueueMessageItem(MessageService.MessageType.FOR_TESTING, TESTING_ACTION);
        enqueueMessageItem(MessageService.MessageType.FOR_TESTING, TESTING_ACTION);

        List<MessageCardProviderMediator.Message> messages =
                mMediator.getReadyMessageItemsForTesting().get(
                        MessageService.MessageType.FOR_TESTING);
        Assert.assertEquals(2, messages.size());
        final MessageCardProviderMediator.Message testingMessage1 = messages.get(0);
        final MessageCardProviderMediator.Message testingMessage2 = messages.get(1);

        messages = mMediator.getMessageItems();
        Assert.assertEquals(1, messages.size());
        Assert.assertEquals(testingMessage1, messages.get(0));

        Assert.assertEquals(1, mMediator.getShownMessageItemsForTesting().size());
        Assert.assertEquals(testingMessage1,
                mMediator.getShownMessageItemsForTesting().get(
                        MessageService.MessageType.FOR_TESTING));

        Assert.assertEquals(1, mMediator.getShownMessageItemsForTesting().size());
        Assert.assertEquals(testingMessage2,
                mMediator.getReadyMessageItemsForTesting()
                        .get(MessageService.MessageType.FOR_TESTING)
                        .get(0));
    }

    @Test
    public void getMessageItemsTest_PersistUntilInvalidationOccurred() {
        enqueueMessageItem(MessageService.MessageType.FOR_TESTING, TESTING_ACTION);
        enqueueMessageItem(MessageService.MessageType.FOR_TESTING, TESTING_ACTION);

        List<MessageCardProviderMediator.Message> messages =
                mMediator.getReadyMessageItemsForTesting().get(
                        MessageService.MessageType.FOR_TESTING);
        Assert.assertEquals(2, messages.size());
        final MessageCardProviderMediator.Message testingMessage1 = messages.get(0);

        // Test message is persisted.
        for (int i = 0; i < 2; i++) {
            messages = mMediator.getMessageItems();
            Assert.assertEquals(1, messages.size());
            Assert.assertEquals(testingMessage1, messages.get(0));
        }

        // Test message updated after invalidation, and the updated message is persisted.
        mMediator.invalidateShownMessage(MessageService.MessageType.FOR_TESTING);
        messages = mMediator.getMessageItems();
        final MessageCardProviderMediator.Message newMessage = messages.get(0);
        Assert.assertEquals(1, messages.size());
        Assert.assertNotEquals(testingMessage1, newMessage);
        for (int i = 0; i < 2; i++) {
            messages = mMediator.getMessageItems();
            Assert.assertEquals(1, messages.size());
            Assert.assertEquals(newMessage, messages.get(0));
        }
    }

    @Test
    public void getMessageItemsTest_ReturnNextMessageIfShownMessageIsInvalided() {
        enqueueMessageItem(MessageService.MessageType.FOR_TESTING, TESTING_ACTION);
        enqueueMessageItem(MessageService.MessageType.FOR_TESTING, TESTING_ACTION);

        List<MessageCardProviderMediator.Message> messages =
                mMediator.getReadyMessageItemsForTesting().get(
                        MessageService.MessageType.FOR_TESTING);
        Assert.assertEquals(2, messages.size());
        final MessageCardProviderMediator.Message testingMessage1 = messages.get(0);
        final MessageCardProviderMediator.Message testingMessage2 = messages.get(1);

        messages = mMediator.getMessageItems();
        Assert.assertEquals(1, messages.size());
        Assert.assertEquals(testingMessage1, messages.get(0));

        mMediator.invalidateShownMessage(MessageService.MessageType.FOR_TESTING);

        messages = mMediator.getMessageItems();
        Assert.assertEquals(1, messages.size());
        Assert.assertEquals(testingMessage2, messages.get(0));

        mMediator.invalidateShownMessage(MessageService.MessageType.FOR_TESTING);
        Assert.assertEquals(0, mMediator.getMessageItems().size());
    }

    @Test
    public void invalidate_allMessages() {
        enqueueMessageItem(
                MessageService.MessageType.TAB_SUGGESTION, TabSuggestion.TabSuggestionAction.CLOSE);

        mMediator.messageInvalidate(MessageService.MessageType.TAB_SUGGESTION);

        Assert.assertFalse(mMediator.getReadyMessageItemsForTesting().containsKey(
                MessageService.MessageType.TAB_SUGGESTION));
        Assert.assertFalse(mMediator.getShownMessageItemsForTesting().containsKey(
                MessageService.MessageType.TAB_SUGGESTION));

        // Testing multiple Messages has the same type.
        enqueueMessageItem(MessageService.MessageType.FOR_TESTING, TESTING_ACTION);
        enqueueMessageItem(MessageService.MessageType.FOR_TESTING, TESTING_ACTION);

        mMediator.messageInvalidate(MessageService.MessageType.FOR_TESTING);
        Assert.assertFalse(mMediator.getReadyMessageItemsForTesting().containsKey(
                MessageService.MessageType.FOR_TESTING));
        Assert.assertFalse(mMediator.getShownMessageItemsForTesting().containsKey(
                MessageService.MessageType.FOR_TESTING));
    }

    @Test
    public void invalidate_shownMessage() {
        enqueueMessageItem(
                MessageService.MessageType.TAB_SUGGESTION, TabSuggestion.TabSuggestionAction.CLOSE);

        mMediator.getMessageItems();
        mMediator.invalidateShownMessage(MessageService.MessageType.TAB_SUGGESTION);

        verify(mUiDismissActionProvider).dismiss(anyInt());
        Assert.assertFalse(mMediator.getShownMessageItemsForTesting().containsKey(
                MessageService.MessageType.TAB_SUGGESTION));
        Assert.assertFalse(mMediator.getReadyMessageItemsForTesting().containsKey(
                MessageService.MessageType.TAB_SUGGESTION));

        // Testing multiple Messages has the same type.
        enqueueMessageItem(MessageService.MessageType.FOR_TESTING, TESTING_ACTION);
        enqueueMessageItem(MessageService.MessageType.FOR_TESTING, TESTING_ACTION);

        mMediator.getMessageItems();
        mMediator.invalidateShownMessage(MessageService.MessageType.FOR_TESTING);
        Assert.assertFalse(mMediator.getShownMessageItemsForTesting().containsKey(
                MessageService.MessageType.FOR_TESTING));
        Assert.assertTrue(mMediator.getReadyMessageItemsForTesting().containsKey(
                MessageService.MessageType.FOR_TESTING));
    }

    @Test
    public void buildModel_ForClosingTabSuggestion() {
        String closingTabSuggestionMessage = "Closing Tab Suggestion";
        doReturn(closingTabSuggestionMessage)
                .when(mContext)
                .getString(R.string.tab_suggestion_close_stale_message);

        enqueueMessageItem(
                MessageService.MessageType.TAB_SUGGESTION, TabSuggestion.TabSuggestionAction.CLOSE);

        PropertyModel model = mMediator.getReadyMessageItemsForTesting()
                                      .get(MessageService.MessageType.TAB_SUGGESTION)
                                      .get(0)
                                      .model;
        Assert.assertEquals(MessageService.MessageType.TAB_SUGGESTION,
                model.get(MessageCardViewProperties.MESSAGE_TYPE));
        Assert.assertEquals(closingTabSuggestionMessage,
                model.get(MessageCardViewProperties.DESCRIPTION_TEXT_TEMPLATE));
    }

    @Test
    public void buildModel_ForGroupingTabSuggestion() {
        String groupingTabSuggestionMessage = "Grouping Tab Suggestion";
        doReturn(groupingTabSuggestionMessage)
                .when(mContext)
                .getString(R.string.tab_suggestion_group_tabs_message);

        enqueueMessageItem(
                MessageService.MessageType.TAB_SUGGESTION, TabSuggestion.TabSuggestionAction.GROUP);

        PropertyModel model = mMediator.getReadyMessageItemsForTesting()
                                      .get(MessageService.MessageType.TAB_SUGGESTION)
                                      .get(0)
                                      .model;
        Assert.assertEquals(MessageService.MessageType.TAB_SUGGESTION,
                model.get(MessageCardViewProperties.MESSAGE_TYPE));
        Assert.assertEquals(groupingTabSuggestionMessage,
                model.get(MessageCardViewProperties.DESCRIPTION_TEXT_TEMPLATE));
    }
}
