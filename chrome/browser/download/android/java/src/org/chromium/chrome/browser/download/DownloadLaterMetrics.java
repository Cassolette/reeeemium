// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.download;

import androidx.annotation.IntDef;

import org.chromium.base.metrics.RecordHistogram;
import org.chromium.chrome.browser.download.dialogs.DownloadLaterDialogChoice;

/**
 * Class that contains helper functions for download later feature metrics recording.
 */
public final class DownloadLaterMetrics {
    /**
     * Defines UI events for download later feature. Used in histograms, don't reuse or remove
     * items. Keep in sync with DownloadLaterUiEvent in enums.xml.
     */
    @IntDef({
            DownloadLaterUiEvent.DOWNLOAD_LATER_DIALOG_SHOW,
            DownloadLaterUiEvent.DOWNLOAD_LATER_DIALOG_COMPLETE,
            DownloadLaterUiEvent.DOWNLOAD_LATER_DIALOG_CANCEL,
            DownloadLaterUiEvent.DATE_TIME_PICKER_SHOW,
            DownloadLaterUiEvent.DATE_TIME_PICKER_COMPLETE,
            DownloadLaterUiEvent.DATE_TIME_PICKER_CANCEL,
            DownloadLaterUiEvent.DOWNLOAD_HOME_CHANGE_SCHEDULE_CLICKED,
            DownloadLaterUiEvent.DOWNLOAD_HOME_CHANGE_SCHEDULE_COMPLETE,
            DownloadLaterUiEvent.DOWNLOAD_HOME_CHANGE_SCHEDULE_CANCEL,
            DownloadLaterUiEvent.DOWNLOAD_INFOBAR_CHANGE_SCHEDULE_CLICKED,
            DownloadLaterUiEvent.DOWNLOAD_INFOBAR_CHANGE_SCHEDULE_COMPLETE,
            DownloadLaterUiEvent.DOWNLOAD_INFOBAR_CHANGE_SCHEDULE_CANCEL,
            DownloadLaterUiEvent.DOWNLOAD_LATER_DIALOG_EDIT_CLICKED,
    })

    public @interface DownloadLaterUiEvent {
        int DOWNLOAD_LATER_DIALOG_SHOW = 0;
        int DOWNLOAD_LATER_DIALOG_COMPLETE = 1;
        int DOWNLOAD_LATER_DIALOG_CANCEL = 2;
        int DATE_TIME_PICKER_SHOW = 3;
        int DATE_TIME_PICKER_COMPLETE = 4;
        int DATE_TIME_PICKER_CANCEL = 5;
        int DOWNLOAD_HOME_CHANGE_SCHEDULE_CLICKED = 6;
        int DOWNLOAD_HOME_CHANGE_SCHEDULE_COMPLETE = 7;
        int DOWNLOAD_HOME_CHANGE_SCHEDULE_CANCEL = 8;
        int DOWNLOAD_INFOBAR_CHANGE_SCHEDULE_CLICKED = 9;
        int DOWNLOAD_INFOBAR_CHANGE_SCHEDULE_COMPLETE = 10;
        int DOWNLOAD_INFOBAR_CHANGE_SCHEDULE_CANCEL = 11;
        int DOWNLOAD_LATER_DIALOG_EDIT_CLICKED = 12;

        int COUNT = 13;
    }

    private DownloadLaterMetrics() {}

    /**
     * Records the user choice on the download later dialog.
     * @param choice The user choice, see {@link DownloadLaterDialogChoice}.
     */
    public static void recordDownloadLaterDialogChoice(@DownloadLaterDialogChoice int choice) {
        RecordHistogram.recordEnumeratedHistogram(
                "Download.Later.UI.DialogChoice.Main", choice, DownloadLaterDialogChoice.COUNT);
        recordDownloadLaterUiEvent(DownloadLaterUiEvent.DOWNLOAD_LATER_DIALOG_COMPLETE);
    }

    /**
     * Records the user choice on the change schedule dialog in download home.
     * @param choice The user choice, see {@link DownloadLaterDialogChoice}.
     */
    public static void recordDownloadHomeChangeScheduleChoice(
            @DownloadLaterDialogChoice int choice) {
        RecordHistogram.recordEnumeratedHistogram("Download.Later.UI.DialogChoice.DownloadHome",
                choice, DownloadLaterDialogChoice.COUNT);
        recordDownloadLaterUiEvent(DownloadLaterUiEvent.DOWNLOAD_HOME_CHANGE_SCHEDULE_COMPLETE);
    }

    /**
     * Records the user choice on the change schedule dialog in download infobar.
     * @param choice The user choice, see {@link DownloadLaterDialogChoice}.
     */
    public static void recordInfobarChangeScheduleChoice(@DownloadLaterDialogChoice int choice) {
        RecordHistogram.recordEnumeratedHistogram(
                "Download.Later.UI.DialogChoice.Infobar", choice, DownloadLaterDialogChoice.COUNT);
        recordDownloadLaterUiEvent(DownloadLaterUiEvent.DOWNLOAD_INFOBAR_CHANGE_SCHEDULE_COMPLETE);
    }

    /**
     * Collects download later UI event metrics.
     * @param event The UI event to collect.
     */
    public static void recordDownloadLaterUiEvent(@DownloadLaterUiEvent int event) {
        RecordHistogram.recordEnumeratedHistogram(
                "Download.Later.UI.Events", event, DownloadLaterUiEvent.COUNT);
    }
}
