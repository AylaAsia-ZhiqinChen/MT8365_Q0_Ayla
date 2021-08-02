package com.mediatek.camera.tests.v3.testcase;

import com.mediatek.camera.common.debug.LogHelper;
import com.mediatek.camera.common.debug.LogUtil;
import com.mediatek.camera.tests.v3.arch.MetaCase;
import com.mediatek.camera.tests.v3.checker.CameraLaunchedChecker;
import com.mediatek.camera.tests.v3.checker.PreviewChecker;
import com.mediatek.camera.tests.v3.operator.DefaultStorageOperator;
import com.mediatek.camera.tests.v3.operator.LaunchCameraOperator;
import com.mediatek.camera.tests.v3.util.Utils;

import org.junit.Before;

public abstract class BaseIntentPhotoTestCase extends BaseTestCase {
    private static final LogUtil.Tag TAG = Utils.getTestTag(
            BaseIntentPhotoTestCase.class.getSimpleName());

    @Before
    public void setUp() {
        LogHelper.d(TAG, "[setUp]");
        super.setUp();
        new MetaCase()
                .addOperator(new DefaultStorageOperator(),
                        DefaultStorageOperator.INDEX_INTERNAL_STORAGE)
                .addOperator(new LaunchCameraOperator(), LaunchCameraOperator.INDEX_INTENT_PHOTO)
                .addChecker(new CameraLaunchedChecker(), CameraLaunchedChecker.INDEX_INTENT_PHOTO)
                .addChecker(new PreviewChecker())
                .run();

    }
}
