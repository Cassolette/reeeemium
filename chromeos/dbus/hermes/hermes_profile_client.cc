// Copyright (c) 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chromeos/dbus/hermes/hermes_profile_client.h"

#include "base/bind.h"
#include "base/memory/weak_ptr.h"
#include "chromeos/dbus/hermes/fake_hermes_profile_client.h"
#include "chromeos/dbus/hermes/hermes_response_status.h"
#include "dbus/bus.h"
#include "dbus/object_manager.h"
#include "dbus/property.h"
#include "third_party/cros_system_api/dbus/hermes/dbus-constants.h"

namespace chromeos {

namespace {
HermesProfileClient* g_instance = nullptr;
}  // namespace

HermesProfileClient::Properties::Properties(
    dbus::ObjectProxy* object_proxy,
    const PropertyChangedCallback& callback)
    : dbus::PropertySet(object_proxy,
                        hermes::kHermesProfileInterface,
                        callback) {
  RegisterProperty(hermes::profile::kIccidProperty, &iccid_);
  RegisterProperty(hermes::profile::kServiceProviderProperty,
                   &service_provider_);
  RegisterProperty(hermes::profile::kMccMncProperty, &mcc_mnc_);
  RegisterProperty(hermes::profile::kActivationCodeProperty, &activation_code_);
  RegisterProperty(hermes::profile::kNameProperty, &name_);
  RegisterProperty(hermes::profile::kNicknameProperty, &nick_name_);
  RegisterProperty(hermes::profile::kStateProperty, &state_);
  RegisterProperty(hermes::profile::kProfileClassProperty, &profile_class_);
}

HermesProfileClient::Properties::~Properties() = default;

class HermesProfileClientImpl : public HermesProfileClient {
 public:
  explicit HermesProfileClientImpl(dbus::Bus* bus) : bus_(bus) {}
  explicit HermesProfileClientImpl(const HermesProfileClient&) = delete;
  ~HermesProfileClientImpl() override = default;

  using Object = std::pair<dbus::ObjectProxy*, Properties*>;
  using ObjectMap = std::map<dbus::ObjectPath, Object>;

  // HermesProfileClient:
  void EnableCarrierProfile(const dbus::ObjectPath& carrier_profile_path,
                            HermesResponseCallback callback) override {
    dbus::MethodCall method_call(hermes::kHermesProfileInterface,
                                 hermes::profile::kEnable);
    dbus::ObjectProxy* object_proxy = GetObject(carrier_profile_path).first;
    object_proxy->CallMethodWithErrorResponse(
        &method_call, dbus::ObjectProxy::TIMEOUT_USE_DEFAULT,
        base::BindOnce(&HermesProfileClientImpl::OnHermesStatusResponse,
                       weak_ptr_factory_.GetWeakPtr(), std::move(callback)));
  }

  void DisableCarrierProfile(const dbus::ObjectPath& carrier_profile_path,
                             HermesResponseCallback callback) override {
    dbus::MethodCall method_call(hermes::kHermesProfileInterface,
                                 hermes::profile::kDisable);
    dbus::ObjectProxy* object_proxy = GetObject(carrier_profile_path).first;
    object_proxy->CallMethodWithErrorResponse(
        &method_call, dbus::ObjectProxy::TIMEOUT_USE_DEFAULT,
        base::BindOnce(&HermesProfileClientImpl::OnHermesStatusResponse,
                       weak_ptr_factory_.GetWeakPtr(), std::move(callback)));
  }

  Properties* GetProperties(
      const dbus::ObjectPath& carrier_profile_path) override {
    return GetObject(carrier_profile_path).second;
  }

  HermesProfileClient& operator=(const HermesProfileClient&) = delete;

 private:
  Object GetObject(const dbus::ObjectPath& object_path) {
    ObjectMap::iterator it = object_map_.find(object_path);
    if (it != object_map_.end())
      return it->second;

    dbus::ObjectProxy* object_proxy =
        bus_->GetObjectProxy(hermes::kHermesServiceName, object_path);

    Properties* properties = new Properties(
        object_proxy,
        base::BindRepeating(&HermesProfileClientImpl::OnPropertyChanged,
                            weak_ptr_factory_.GetWeakPtr(), object_path));
    properties->ConnectSignals();
    properties->GetAll();

    Object object = std::make_pair(object_proxy, properties);
    object_map_[object_path] = object;
    return object;
  }

  void OnPropertyChanged(const dbus::ObjectPath& object_path,
                         const std::string& property_name) {
    for (auto& observer : observers()) {
      observer.OnCarrierProfilePropertyChanged(object_path, property_name);
    }
  }

  void OnHermesStatusResponse(HermesResponseCallback callback,
                              dbus::Response* response,
                              dbus::ErrorResponse* error_response) {
    if (error_response) {
      std::move(callback).Run(
          HermesResponseStatusFromErrorName(error_response->GetErrorName()));
      return;
    }
    std::move(callback).Run(HermesResponseStatus::kSuccess);
  }

  dbus::Bus* bus_;
  ObjectMap object_map_;
  base::WeakPtrFactory<HermesProfileClientImpl> weak_ptr_factory_{this};
};

HermesProfileClient::HermesProfileClient() {
  DCHECK(!g_instance);
  g_instance = this;
}

HermesProfileClient::~HermesProfileClient() {
  DCHECK_EQ(g_instance, this);
  g_instance = nullptr;
}

void HermesProfileClient::AddObserver(Observer* observer) {
  DCHECK(observer);
  observers_.AddObserver(observer);
}

void HermesProfileClient::RemoveObserver(Observer* observer) {
  DCHECK(observer);
  observers_.RemoveObserver(observer);
}

// static
void HermesProfileClient::Initialize(dbus::Bus* bus) {
  DCHECK(bus);
  DCHECK(!g_instance);
  new HermesProfileClientImpl(bus);
}

// static
void HermesProfileClient::InitializeFake() {
  new FakeHermesProfileClient();
}

// static
void HermesProfileClient::Shutdown() {
  DCHECK(g_instance);
  delete g_instance;
}

// static
HermesProfileClient* HermesProfileClient::Get() {
  return g_instance;
}

}  // namespace chromeos
