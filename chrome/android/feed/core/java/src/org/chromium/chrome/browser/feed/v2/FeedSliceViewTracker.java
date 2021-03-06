// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.feed.v2;

import android.graphics.Rect;
import android.view.View;
import android.view.ViewTreeObserver;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.VisibleForTesting;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import java.util.HashSet;

/**
 * Tracks position of slice views. When a slice's view is first 2/3rds visible in the viewport,
 * the observer is notified.
 */
class FeedSliceViewTracker implements ViewTreeObserver.OnPreDrawListener {
    private static final String TAG = "FeedSliceViewTracker";
    private static final double DEFAULT_VIEW_LOG_THRESHOLD = .66;

    @Nullable
    private RecyclerView mRootView;
    @Nullable
    private FeedListContentManager mContentManager;
    // The set of content keys already reported as visible.
    private HashSet<String> mContentKeysVisible = new HashSet<String>();
    @Nullable
    private Observer mObserver;

    /** Notified the first time slices are visible */
    public interface Observer {
        void sliceVisible(String sliceId);
    }

    FeedSliceViewTracker(@NonNull RecyclerView rootView,
            @NonNull FeedListContentManager contentManager, @NonNull Observer observer) {
        mRootView = (RecyclerView) rootView;
        mContentManager = contentManager;
        mObserver = observer;
        mRootView.getViewTreeObserver().addOnPreDrawListener(this);
    }

    /** Stop observing rootView. Prevents further calls to observer. */
    public void destroy() {
        if (mRootView != null && mRootView.getViewTreeObserver().isAlive()) {
            mRootView.getViewTreeObserver().removeOnPreDrawListener(this);
        }
        mRootView = null;
        mObserver = null;
        mContentManager = null;
    }

    // ViewTreeObserver.OnPreDrawListener.
    @Override
    public boolean onPreDraw() {
        // Not sure why, but this method can be called just after destroy().
        if (mRootView == null) return true;
        if (!(mRootView.getLayoutManager() instanceof LinearLayoutManager)) return true;

        LinearLayoutManager layoutManager = (LinearLayoutManager) mRootView.getLayoutManager();
        int firstPosition = layoutManager.findFirstVisibleItemPosition();
        int lastPosition = layoutManager.findLastVisibleItemPosition();
        for (int i = firstPosition;
                i <= lastPosition && i < mContentManager.getItemCount() && i >= 0; ++i) {
            String contentKey = mContentManager.getContent(i).getKey();
            View childView = layoutManager.findViewByPosition(i);
            if (mContentKeysVisible.contains(contentKey) || childView == null
                    || !isViewVisible(childView)) {
                continue;
            }
            mContentKeysVisible.add(contentKey);
            mObserver.sliceVisible(contentKey);
        }
        return true;
    }

    @VisibleForTesting
    boolean isViewVisible(View childView) {
        Rect rect = new Rect(0, 0, childView.getWidth(), childView.getHeight());
        int viewArea = rect.width() * rect.height();
        if (viewArea <= 0) return false;
        if (!mRootView.getChildVisibleRect(childView, rect, null)) return false;
        int visibleArea = rect.width() * rect.height();
        return (float) visibleArea / viewArea >= DEFAULT_VIEW_LOG_THRESHOLD;
    }
}
