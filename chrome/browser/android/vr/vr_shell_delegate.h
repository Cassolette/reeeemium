// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_ANDROID_VR_VR_SHELL_DELEGATE_H_
#define CHROME_BROWSER_ANDROID_VR_VR_SHELL_DELEGATE_H_

#include <jni.h>

#include <map>
#include <memory>

#include "base/android/jni_weak_ref.h"
#include "base/callback.h"
#include "base/cancelable_callback.h"
#include "base/macros.h"
#include "chrome/browser/android/vr/vr_core_info.h"
#include "content/public/browser/xr_runtime_manager.h"
#include "device/vr/android/gvr/gvr_delegate_provider.h"
#include "device/vr/public/mojom/vr_service.mojom.h"
#include "device/vr/vr_device.h"
#include "third_party/gvr-android-sdk/src/libraries/headers/vr/gvr/capi/include/gvr_types.h"

namespace device {
class GvrDevice;
}

namespace vr {

// GENERATED_JAVA_ENUM_PACKAGE: org.chromium.chrome.browser.vr
enum class VrSupportLevel : int {
  kVrDisabled = 0,
  kVrNeedsUpdate = 1,  // VR Support is available, but needs update.
  kVrCardboard = 2,
  kVrDaydream = 3,  // Supports both Cardboard and Daydream viewer.
};

class VrShell;

class VrShellDelegate : public device::GvrDelegateProvider,
                        content::XRRuntimeManager::Observer {
 public:
  VrShellDelegate(JNIEnv* env, jobject obj);
  ~VrShellDelegate() override;

  static device::GvrDelegateProvider* CreateVrShellDelegate();

  static VrShellDelegate* GetNativeVrShellDelegate(
      JNIEnv* env,
      const base::android::JavaRef<jobject>& jdelegate);

  void SetDelegate(VrShell* vr_shell);
  void RemoveDelegate();

  void SetPresentResult(JNIEnv* env,
                        const base::android::JavaParamRef<jobject>& obj,
                        jboolean success);
  void OnPause(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj);
  void OnResume(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj);
  bool IsClearActivatePending(JNIEnv* env,
                              const base::android::JavaParamRef<jobject>& obj);
  void Destroy(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj);

  device::GvrDevice* GetGvrDevice();

  void SendRequestPresentReply(device::mojom::XRSessionPtr session);

  // device::GvrDelegateProvider implementation.
  void ExitWebVRPresent() override;

 private:
  // device::GvrDelegateProvider implementation.
  bool ShouldDisableGvrDevice() override;
  void StartWebXRPresentation(
      device::mojom::VRDisplayInfoPtr display_info,
      device::mojom::XRRuntimeSessionOptionsPtr options,
      base::OnceCallback<void(device::mojom::XRSessionPtr)> callback) override;

  // content::XRRuntimeManager::Observer implementation.
  // VrShellDelegate implements XRRuntimeManager::Observer to turn off poses (by
  // calling SetInlinePosesEnabled) on a runtime that gets initialized and added
  // to XRRuntimeManager, while the VrShell is active (user has headset on).
  // As for the runtimes that got added to the XRRuntimeManager before the
  // VrShell got created, their poses will be turned off too on its
  // creation.
  void OnRuntimeAdded(content::BrowserXRRuntime* runtime) override;
  void OnPresentResult(
      device::mojom::VRDisplayInfoPtr display_info,
      device::mojom::XRRuntimeSessionOptionsPtr options,
      base::OnceCallback<void(device::mojom::XRSessionPtr)> callback,
      bool success);

  std::unique_ptr<VrCoreInfo> MakeVrCoreInfo(JNIEnv* env);

  base::android::ScopedJavaGlobalRef<jobject> j_vr_shell_delegate_;
  VrShell* vr_shell_ = nullptr;

  // Deferred callback stored for later use in cases where vr_shell
  // wasn't ready yet. Used once SetDelegate is called.
  base::OnceCallback<void(bool)> on_present_result_callback_;

  // Mojo callback waiting for request present response. This is temporarily
  // stored here from OnPresentResult's outgoing ConnectPresentingService call
  // until the reply arguments are received by SendRequestPresentReply.
  base::OnceCallback<void(device::mojom::XRSessionPtr)>
      request_present_response_callback_;

  bool pending_successful_present_request_ = false;

  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

  base::WeakPtrFactory<VrShellDelegate> weak_ptr_factory_{this};

  DISALLOW_COPY_AND_ASSIGN(VrShellDelegate);
};

}  // namespace vr

#endif  // CHROME_BROWSER_ANDROID_VR_VR_SHELL_DELEGATE_H_
